/* 
 * File:   fonctions.h
 * Author: pehladik
 *
 * Created on 12 janvier 2012, 10:19
 */

#ifndef FONCTIONS_H
#define	FONCTIONS_H

#include "global.h"
#include "includes.h"

#ifdef	__cplusplus
extern "C" {
#endif
        void connecter (void * arg);
        void communiquer(void *arg);
        void deplacer(void *arg);
        void envoyer(void *arg);
	void traiter_image(void *arg);
	void battery_level(void *arg);
	//void fermeture_connexion_robot(void *arg);
	void detecter_arene (void * arg);
	//void calcul_pos(void *arg);

	void connecter_moniteur(void *arg) ;
	//void traiter_ordre(void *arg) ;

	int write_in_queue(RT_QUEUE *msgQueue, void * data, int size);
#ifdef	__cplusplus
}
#endif

#endif	/* FONCTIONS_H */
