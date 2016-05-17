/*
 * File:   global.c
 * Author: pehladik
 *
 * Created on 21 avril 2011, 12:14
 */

#include "global.h"

RT_TASK tServeur;
RT_TASK tconnect;
RT_TASK tmove;
RT_TASK tenvoyer;
RT_TASK ttraiter_image;
//RT_TASK tdetecter_arene;
//RT_TASK tbattery_level;
//RT_TASK tfermeture_connexion_robot;

RT_MUTEX mutexEtat;
RT_MUTEX mutexMove;

RT_SEM semConnecterRobot;

RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int etatCommRobot = 1;
int img_transmit = 0;
DRobot *robot;
DMovement *move;
DServer *serveur;

//DBattery *battery;

int MSG_QUEUE_SIZE = 10;

int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 10;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TIMAGE = 20;
//int PRIORITY_TARENE = 23;
//int PRIORITY_TBATTERY = 10;
//int PRIORITY_TFERMER_ROBOT = 35;
