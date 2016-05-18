/* 
 * File:   global.h
 * Author: pehladik
 *
 * Created on 12 janvier 2012, 10:11
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include "includes.h"

/* @descripteurs des tâches */
extern RT_TASK tServeur;
extern RT_TASK tconnect;
extern RT_TASK tmove;
extern RT_TASK tenvoyer;
extern RT_TASK ttraiter_image;
//extern RT_TASK tconnecter_moniteur
//extern RT_TASK ttraiter_ordre

//extern RT_TASK tdetecter_arene ;
//extern RT_TASK tbattery_level;
//extern RT_TASK tcalcul_pos;
//extern RT_TASK tfermeture_connexion_robot ;

/* @descripteurs des mutex */
extern RT_MUTEX mutexServeur;
extern RT_MUTEX mutexEtatMon;
extern RT_MUTEX mutexRobot;
extern RT_MUTEX mutexMove;
extern RT_MUTEX mutexArene;
extern RT_MUTEX mutexBatterie;
extern RT_MUTEX mutexPosition;
extern RT_MUTEX mutexTransmit;

/* @descripteurs des sempahore */
extern RT_SEM semConnecterRobot;
extern RT_SEM semConnexionEtablie;
extern RT_SEM semConnexionEchouee ;
extern RT_SEM semLancerPosition ;
extern RT_SEM semTrouverArene ;






/* @descripteurs des files de messages */
extern RT_QUEUE queueMsgGUI;

/* @variables partagées */
extern int etatCommMoniteur;
extern int img_transmit;
extern DServer *serveur;
extern DRobot *robot;
extern DMovement *move;
extern DArena *arena;
//extern DBattery *battery;
//extern DPosition *position;

/* @constantes */
extern int MSG_QUEUE_SIZE;
extern int PRIORITY_TSERVEUR;
extern int PRIORITY_TCONNECT;
extern int PRIORITY_TMOVE;
extern int PRIORITY_TENVOYER;
extern int PRIORITY_TIMAGE;
//extern int PRIORITY_TARENE;
//extern int PRIORITY_TBATTERY;
//extern int PRIORITY_TFERMER_ROBOT;
//extern int PRIORITY_TPOSITION;

#endif	/* GLOBAL_H */
