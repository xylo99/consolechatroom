# Installation

Just run the script demo.sh

# Prereqs

Docker

# Feature

Now with ncurses! Chat with 10 other peers!

# Explanation

Two windows will open and a prompt will appear. In total there will be three terminals: one that lauched the demo script, and two for the client sessions.
In one of the client terminals, simply type your message into the prompt, then the other client will receive that message. 
Once the other client is satisfied with that message, they will send a message back, to which you will receive.

## In new terminal window

```
<type message><press enter>
```

## To end the demo 

```
In the terminal in which ./demo.sh was ran, enter ctrl-c.
```

# Info

This is a simple, lightweight chatroom written in C. Chatting is synchronous, meaning that only one client can send a message at a time.
