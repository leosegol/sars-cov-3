# sars-cov-3
sars-cov-3 is my a project for my finals.
This virus spreads in networks using DHCP spoofing and DNS hijacking. 
The attacker replaces certain sites with his site which says the site has a problem and an application must be downloaded and ran to fix it.
When the application is ran for the first time it copies itself to the startup menu of the computer and runs in the background.
When the bot master connects to the same network as a victim they connect, and the master has the option to choose the victim and send Shell commands which will be ran
by the bot.

The infection is written in c++ because of efficiency, speed is very important in this part because the attacker has to race the router and send his DHCP ACK packet
before the router does. It then has to answer and forward all DNS traffic.

The Victim, virus, is written in c++ aswell because it has to take as little resources as possible so the user wont notice it is there. 
It is also letting the virus pass undetected by the antivirus.

The http server is written in python, it doesn't have a lot of traffic so efficiency is not very important and will suffice, the bot master is also written in python
because it doesnt need to be fast and it's a lot simpler to program and control.

The bot master has special commands which are not Shell, axplained better in server's readme.


The infecting must have the npcap dlls, you can download it here. https://nmap.org/download.html
