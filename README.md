# Coin Catcher

---

### Introduction

This project is aimed to be a simple implementation of some coin logic. When the player overlaps
with the coin, the coin counter is incremented, and when the total number of coins is reached
(i.e. there are no coins left on the map) a win condition is triggered.

This project focuses on two main C++ Classes. CC_CoinPickupActor and CoinCatcherCharacter

---

### Coin Pickup Actor

With this class, we add a Static Mesh Component, a Rotating Movement Component, Collider Component
(sphere), then a Niagara componenet, which is our On Pickup Effect. Also we have a sound which is 
played When the player overlaps the collider, the Niagara effect is spawned, sound is played, and 
then the coin is destroyed. 

---

### Coin Catcher Character

This is a regular third person template, with a few key differences. 

We added the following Attributes and Properties:

    CollectedCoins -- Number of coins collected (int32)
    TotalCoins -- Total number of coins spawned in the map (int32)
    IsCoinCollected -- A bool which is used in the collection logic
    WinPopupWidgetClass -- A reference to the UMG Widget which is displayed on win condition

    GetCollectedCoins() -- Getter for Collected Coins
    UpdateTotalCoins() -- Run at initialization, to determine how many coins are in the map
    CollectCoin() -- Collects and destroys the coin
    UpdateCoinCount() -- Self explanitory
    TriggerWin() -- Run after all coins are collected, and displays a "You Win" Message
