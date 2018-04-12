# HOLD - ReadMe
## Description
A Broodwar AI developed in c++ using BWAPI and BWEM.

## Why do I write Starcraft bots/AIs?
The most important and difficult task in creating a strategy game is the creation of fun AI opponent. I can experiment with all things I've learned in the meantime; problem solving, software engineering, data structure and AI. It was so nice applying these techniques into commercial level game rather than academic projects.

## Engine Architecture
The structure of the AI is called Head Quarter and the hierarchy is similar to the real army structure. There are several commands under the head quarter. The current AI consists largely of two commands. The commander of the higher unit orders the lower unit, and keep ordering until the command is received by lowest unit. First, there is a logistic command. The real command supports the unit and manages the resources. In contrast to the built-in AI knowing all the information on the battlefield (like the map hack), the AI I'm trying to make was like real people playing, so I can only remember what I saw, and there was a lot of information missing due to the fog of war. The first thing I tried was to store and manage all the units' information. The time and location of the other player's disappearance were stored and the risk of the location was calculated. Each unit had its own unique ID. To make this possible, I created a new data structure and made it possible to reuse existing APIs in a wrapper format. Another important feature is the build order. It will also be important to pull units while managing resources efficiently. To do this, orders were stored using a priority queue. The problem here is how to prioritize, I think a little more research is needed on this part. The Zerg tribe does not have a production standby function and produces a special form that transforms a limited number of units called Lavar. So I researched and followed what the pro-gamer have done so far.

The other major command is Intelligence Command. Within a game, it determine the behavior of all units. It uses a technique called influence map to attack and evade. Briefly, the map is divided into a grid of WalkPosition size, which is the minimum movement unit of the game, and the influence of units on each grid is calculated. This can be used to tell where the battle will take place and where to send defense forces. Under the intelligence command, there is a scouter that has the ability to arrange the start positions in a clockwise order based on the opinion of pro gamers. It will also be necessary to manage units efficiently in the process of engaging. It was implemented with the advice of professor Benjamin Ellinger. By comparing the opponent unit with my units in some radius, it can obtain killing time, and attack the enemy that takes the shortest time, which gives the advantage to the AI who use individual battle. The problem found here is latency, which was an unexpected problem. I wanted to do it during the semester, but I could not fix it because of lacked network knowledge, was extremely complex internally, and it was difficult to have an environment to test. In order to reduce the variables in the beginning, a certain strategy is set up for the starting. In order to spread more effective tactics to the opponent, a log system was created. For each map, the result of each starting strategies were stored and choose higher strategy for the next battle. In addition, when the game is played with the opponent for the first time, the strategy with the highest one was used based on the existing statistics.


**Influence Map**

![](https://github.com/yeongki/yeongki.github.io/blob/master/img/fig1.png)

Figure 1 : My influence map is on green grid. All influence is coming from my units, buildings etc.


![](https://github.com/yeongki/yeongki.github.io/blob/master/img/fig2.png)

Figure 2 : Opponent influence map is on red grid. All influence is coming from opposing units, buildings etc.


![](https://github.com/yeongki/yeongki.github.io/blob/master/img/fig3.png)

Figure 3 : Tension map is on purple grid. Calculated as MY influence + Opponent influence. We can find where the huge battle will take place by using tension map.


![](https://github.com/yeongki/yeongki.github.io/blob/master/img/fig4.png)

Figure 4 : Vulnerability map is on white grid. Calculated as Tension map – Abs(Influence map). This tells where your opponent is vulnerable for attack.


![](https://github.com/yeongki/yeongki.github.io/blob/master/img/fig5.png)

Figure 5 : This shows how the influence map is used to kite. The flying units tried to find a safe path and run away after attacking weak units.

## Achievement
![](https://github.com/yeongki/yeongki.github.io/blob/master/img/feb14.2018_6th.jpg)

<img src="https://sscaitournament.com/images/achievements/equalOpportunity.png" width="50"> Equal opportunity : Win at least one game against all 3 races.

<img src="https://sscaitournament.com/images/achievements/winningStreak3.png" width="50"> Winning Streak 3 : Win 3 games in a row.

<img src="https://sscaitournament.com/images/achievements/winningStreak5.png" width="50"> Winning Streak 5 : Win 5 games in a row.

<img src="https://sscaitournament.com/images/achievements/experienced.png" width="50"> Experienced : Have at least 50% win rate over 4 days.

<img src="https://sscaitournament.com/images/achievements/veteran.png" width="50"> Veteran : Have at least 65% win rate over 4 days.

## Reference
•	Influence Maps, https://gameschoolgems.blogspot.com/2009/12/influence-maps-i.html

