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
//RT_TASK tconnecter_moniteur
//RT_TASK ttraiter_ordre
//RT_TASK tdetecter_arene;
//RT_TASK tbattery_level;
//RT_TASK tfermeture_connexion_robot;
//RT_TASK tcalcul_pos;

RT_MUTEX mutexServeur;
RT_MUTEX mutexEtatMon;
RT_MUTEX mutexRobot;
RT_MUTEX mutexMove;
RT_MUTEX mutexArene;
RT_MUTEX mutexBatterie;
RT_MUTEX mutexPosition;
RT_MUTEX mutexTransmit;

RT_SEM semConnecterRobot;
RT_SEM semConnexionEtablie;
RT_SEM semConnexionEchouee ;
RT_SEM semLancerPosition ;
RT_SEM semTrouverArene ;

RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int img_transmit = 0;
DRobot *robot;
DMovement *move;
DServer *serveur;
DArena *arena;

//DBattery *battery;
//DPosition *position;

int MSG_QUEUE_SIZE = 10;

int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 10;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TIMAGE = 20;
//int PRIORITY_TCONECTER_MONITEUR = FORTES ;
//int PRIORITY_TTRAITER_ORDRE = FORTE+ ;
//int PRIORITY_TARENE = 23;
//int PRIORITY_TBATTERY = 10;
//int PRIORITY_TPOSITION = 20;
//int PRIORITY_TFERMER_ROBOT = 35;
