/*****************************************************************************/
/*!
\file   BehaviorTree.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <initializer_list>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <random>

namespace HOLD
{
	class BehaviorTree
	{
	public:
		class Node
		{
		public:
			virtual bool run() = 0;
		};

		class CompositeNode : public Node
		{
		private:
			std::vector<Node*> children;
		public:
			const std::vector<Node*>& getChildren() const { return children; }
			void addChild(Node* child) { children.emplace_back(child); }
			void addChildren(std::initializer_list<Node*>&& newChildren) { for (Node* child : newChildren) addChild(child); }
			template <typename CONTAINER>
			void addChildren(const CONTAINER& newChildren) { for (Node* child : newChildren) addChild(child); }
		protected:
			//todo: fix shuffle
			//std::vector<Node*> childrenShuffled() const { std::vector<Node*> temp = children;  std::shuffle(temp.begin(), temp.end(), std::mt19937{ std::random_device() });  return temp; }
			//std::vector<Node*> childrenShuffled() const { std::vector<Node*> temp = children;  std::random_shuffle(temp.begin(), temp.end());  return temp; }
			std::vector<Node*> childrenShuffled() const { return children; }
		};

		class Selector : public CompositeNode {
		public:
			virtual bool run() override {
				for (Node* child : getChildren())
				{
					if (child->run())
						return true;
				}
				return false;
			}
		};

		class RandomSelector : public CompositeNode
		{
		public:
			virtual bool run() override {
				for (Node* child : childrenShuffled())
				{
					if (child->run())
						return true;
				}
				return false;
			}
		};

		class Sequence : public CompositeNode {
		public:
			virtual bool run() override {
				for (Node* child : getChildren())
				{
					if (!child->run())
						return false;
				}
				return true;
			}
		};

		class DecoratorNode : public Node
		{
		private:
			Node* child; // It should be only one
		protected:
			Node* getChild() const { return child; }
		public:
			void setChild(Node* newChild) { child = newChild; }
		};

		class Root : public DecoratorNode
		{
		private:
			friend class BehaviorTree;
			virtual bool run() override { return getChild()->run(); }
		};

		class Inverter : public DecoratorNode
		{
		private:
			virtual bool run() override { return !getChild()->run(); }
		};

		class Succeeder : public DecoratorNode
		{
		private:
			virtual bool run() override { getChild()->run();  return true; }
		};

		class Failer : public DecoratorNode
		{
		private:
			virtual bool run() override { getChild()->run();  return false; }
		};

		class Repeater : public DecoratorNode
		{
		private:
			int numRepeats;
			static const int NOT_FOUND = -1;
			Repeater(int num = NOT_FOUND) : numRepeats(num) {}
			virtual bool run() override {
				if (numRepeats == NOT_FOUND)
					while (true) getChild()->run();
				else {
					for (int i = 0; i < numRepeats - 1; i++)
						getChild()->run();
					return getChild()->run();
				}
			}
		};

		class RepeatUntilFail : public DecoratorNode
		{
		private:
			virtual bool run() override {
				while (getChild()->run()) {}
				return true;
			}
		};

		template <typename T>
		class StackNode : public Node
		{
		protected:
			std::stack<T*>& stack;
			StackNode(std::stack<T*>& s) : stack(s) {}
		};

		template <typename T>
		class PushToStack : public StackNode<T>
		{
		private:
			T*& item;
		public:
			PushToStack(T*& t, std::stack<T*>& s) : StackNode<T>(s), item(t) {}
		private:
			virtual bool run() override {
				this->stack.push(item);
				return true;
			}
		};

		template <typename T>
		class GetStack : public StackNode<T>
		{
		private:
			const std::stack<T*>& obtainedStack;
			T* object;
		public:
			GetStack(std::stack<T*>& s, const std::stack<T*>& o, T* t = nullptr) : StackNode<T>(s), obtainedStack(o), object(t) {}
		private:
			virtual bool run() override {
				this->stack = obtainedStack;
				if (object)
					this->stack.push(object);
				return true;
			}
		};

		template <typename T>
		class PopFromStack : public StackNode<T>
		{
		private:
			T*& item;
		public:
			PopFromStack(T*& t, std::stack<T*>& s) : StackNode<T>(s), item(t) {}
		private:
			virtual bool run() override {
				if (this->stack.empty())
					return false;
				item = this->stack.top();
				this->stack.pop();
				return true;
			}
		};

		template <typename T>
		class StackIsEmpty : public StackNode<T>
		{
		public:
			StackIsEmpty(std::stack<T*>& s) : StackNode<T>(s) {}
		private:
			virtual bool run() override {
				return this->stack.empty();
			}
		};

		template <typename T>
		class SetVariable : public BehaviorTree::Node
		{
		private:
			T *&variable, *&object;
		public:
			SetVariable(T*& t, T*& obj) : variable(t), object(obj) {}
			virtual bool run() override {
				variable = object;
				return true;
			};
		};

		template <typename T>
		class IsNull : public BehaviorTree::Node
		{
		private:
			T*& object;  // Must use reference to pointer to work correctly.
		public:
			IsNull(T*& t) : object(t) {}
			virtual bool run() override { return !object; }
		};
	public:
		Root* root;
	public:
		BehaviorTree() : root(new Root) {}
		void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
		bool run() const { return root->run(); }
	};
}
