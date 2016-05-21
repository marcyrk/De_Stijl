#include "fonctions.h"
#define ATTENTE_CONNEXION 0
#define TEST_CONNEXION 1
#define TEST_RECEPTION 2
#define TEST_LECTURE 3


int write_in_queue(RT_QUEUE *msgQueue, void * data, int size);

//==========================================================================

// fonctionnnement OK

void connecter_moniteur(void *arg) {

    int statut_serveur ;
    rt_printf("tserver : Début de l'exécution de serveur\n");

    rt_mutex_acquire(&mutexServeur, TM_INFINITE);
    statut_serveur = serveur->open(serveur, "8000") ;
    rt_mutex_release(&mutexServeur) ;


    rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
    etatCommMoniteur = statut_serveur;
    rt_mutex_release(&mutexEtatMon);

      if (statut_serveur == 0){
      rt_printf("tserver : Connexion reussie\n");
       rt_sem_v(&semConnexionEtablie);
    }
      else{
	rt_printf("tserver : Connexion échouée\n");
	rt_sem_v(&semConnexionEchouee);
      }

}
//==========================================================================


// à tester
void traiter_ordre(void *arg) {
	int evolution = TEST_CONNEXION;
	int nb_octets_recu;
	DMessage *msg = d_new_message();
	int statut_serveur;
	DAction *monAction = d_new_action();
	// test
	int num_msg = 0;
while(1){
	// Verifier le partage de la variable serveur
	if(evolution == ATTENTE_CONNEXION){
    rt_printf("Attente de connexion\n");
    rt_sem_p(&semConnexionEtablie, TM_INFINITE);
    rt_printf("Connexion reussie\n");
    evolution = TEST_CONNEXION;
    }else if(evolution == TEST_CONNEXION){
        rt_mutex_acquire(&mutexServeur, TM_INFINITE);
        statut_serveur = d_server_is_active(serveur) ;
        rt_mutex_release(&mutexServeur) ;
            if ( statut_serveur!= 0){
            rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
            etatCommMoniteur = 0;
            rt_mutex_release(&mutexEtatMon) ;
            evolution = TEST_RECEPTION ;
            }
            else{
            rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
            etatCommMoniteur = -1;
            rt_mutex_release(&mutexEtatMon) ;
            evolution = ATTENTE_CONNEXION ;
            }
    }else if(evolution == TEST_RECEPTION){
        rt_mutex_acquire(&mutexServeur, TM_INFINITE);
        nb_octets_recu = d_server_receive(serveur,msg) ;
        rt_mutex_release(&mutexServeur) ;
        evolution = TEST_LECTURE ;
    }
    else if(evolution == TEST_LECTURE){
          switch(msg->get_type(msg)) {
            case MESSAGE_TYPE_MOVEMENT:
                rt_mutex_acquire(&mutexMove, TM_INFINITE);
                move->from_message(move,msg) ;
                rt_mutex_release(&mutexMove) ;
                evolution = TEST_CONNEXION ;
                break;
            case MESSAGE_TYPE_ACTION:
                //DAction *monAction = d_new_action();
                rt_printf("tserver : Le message %d reçu est une action\n",
                            num_msg);
				monAction->from_message(monAction,msg);
				rt_printf("COUCOU ACTION DEMANDÉE EST ");
				rt_printf(monAction);
				rt_printf("COUCOU C'ETAIT L'ACTION DEMANDÉE");
                switch(monAction->get_order(monAction)){
                    case ACTION_COMPUTE_CONTINUOUSLY_POSITION:
                        rt_sem_v(&semLancerPosition);
                        evolution = TEST_CONNEXION ;
                    break ;
                    case ACTION_CONNECT_ROBOT :
                        rt_sem_v(&semConnecterRobot);
                        evolution = TEST_CONNEXION ;
                    break ;
                    case ACTION_FIND_ARENA:
                        rt_sem_v(&semTrouverArene) ;
                        evolution = TEST_CONNEXION ;
                    break ;
                }
            }
        }
    }
}


//==========================================================================

void envoyer(void * arg) {
    DMessage *msg;
    int err;

    while (1) {
        rt_printf("tenvoyer : Attente d'un message\n");
        if ((err = rt_queue_read(&queueMsgGUI, &msg, sizeof (DMessage), TM_INFINITE)) >= 0) {
	  rt_printf("tenvoyer : envoi d'un message au moniteur\n");
	  rt_mutex_acquire(&mutexServeur, TM_INFINITE);
	  serveur->send(serveur, msg);
	  rt_mutex_release(&mutexServeur);
	  msg->free(msg);
        } else {
            rt_printf("Error msg queue write: %s\n", strerror(-err));
        }
    }
}

//==========================================================================

// problème avec les mutex !!! impossible de rentrer dans le sémaphore d'ouverture de connexion avec le robot

void connecter(void * arg) {
    int status = 0;
    DMessage *message;

    rt_printf("tconnect : Debut de l'exécution de tconnect\n");

    while (1) {
        rt_printf("tconnect : Attente du sémarphore semConnecterRobot\n");
        rt_sem_p(&semConnecterRobot, TM_INFINITE);
        rt_printf("tconnect : Ouverture de la communication avec le robot\n");
	rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	rt_printf("tconnect : mutex acquired\n");
        status = robot->open_device(robot);

        if (status == STATUS_OK) {
            status = robot->start_insecurely(robot);
            if (status == STATUS_OK){
                rt_printf("tconnect : Robot démarrer\n");
            }
        }
	rt_mutex_release(&mutexRobot);
	rt_printf("tconnect : mutex released\n");

        message = d_new_message();
        message->put_state(message, status);

        rt_printf("tconnecter : Envoi message\n");
        message->print(message, 100);

        if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
            message->free(message);
        }
    }
}

//==========================================================================

void communiquer(void *arg) {
    DMessage *msg = d_new_message();
    int var1 = 1;
    int num_msg = 0;
/*
	// traité par connexion moniteur => à supprimer
    rt_printf("tserver : Début de l'exécution de serveur\n");
    serveur->open(serveur, "8000");
    rt_printf("tserver : Connexion\n");
*/
    rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
    etatCommMoniteur = 0;
    rt_mutex_release(&mutexEtatMon);

    while (var1 > 0) {
      rt_printf("tserver : Attente d'un message\n");
      rt_mutex_acquire(&mutexServeur, TM_INFINITE);
      var1 = serveur->receive(serveur, msg);
      rt_mutex_release(&mutexServeur);
      num_msg++;
      if (var1 > 0) {
            switch (msg->get_type(msg)) {
                case MESSAGE_TYPE_ACTION:
                    rt_printf("tserver : Le message %d reçu est une action\n",
                            num_msg);
                    DAction *action = d_new_action();
                    action->from_message(action, msg);
                    switch (action->get_order(action)) {
                        case ACTION_CONNECT_ROBOT:
                            rt_printf("tserver : Action connecter robot\n");
                            rt_sem_v(&semConnecterRobot);		// PROBLÈME ICI !
                            break;
                    }
                    break;
                case MESSAGE_TYPE_MOVEMENT:
                    rt_printf("tserver : Le message reçu %d est un mouvement\n",
                            num_msg);
                    rt_mutex_acquire(&mutexMove, TM_INFINITE);
                    move->from_message(move, msg);
                    move->print(move);
                    rt_mutex_release(&mutexMove);
                    break;
            }
        }
    }
}

//==========================================================================

void deplacer(void *arg) {
    int status = 1;
    int gauche;
    int droite;
    DMessage *message;

    rt_printf("tmove : Debut de l'éxecution de periodique à 1s\n");
    rt_task_set_periodic(NULL, TM_NOW, 1000000000);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_printf("tmove : Activation périodique\n");

        rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	status = robot->get_status(robot);
	rt_mutex_release(&mutexRobot);

        if (status == STATUS_OK) {
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
            switch (move->get_direction(move)) {
                case DIRECTION_FORWARD:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_LEFT:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
                case DIRECTION_RIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_STOP:
                    gauche = MOTEUR_STOP;
                    droite = MOTEUR_STOP;
                    break;
                case DIRECTION_STRAIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
            }
            rt_mutex_release(&mutexMove);

	    rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	    status = robot->set_motors(robot, gauche, droite);
	    rt_mutex_release(&mutexRobot);

            if (status != STATUS_OK) {

                message = d_new_message();
                message->put_state(message, status);

                rt_printf("tmove : Envoi message\n");
                if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
                    message->free(message);
                }
            }
        }
    }
}

/*
void deplacer(void *arg) {
    int status = 1;
    int gauche;
    int droite;
    float angle = 0.0;
    int vitesse_mouvement = 0;
    char type_msg;
    DMessage *message;
    rt_printf("tmove : Debut de l'éxecution de periodique à 200ms\n");
    rt_task_set_periodic(NULL, TM_NOW, 200000000);
    while (1) {
        // Attente de l'activation périodique
        rt_task_wait_period(NULL);
        rt_printf("tmove : Activation périodique\n");
        rt_mutex_acquire(&mutexRobot, TM_INFINITE);
        status = robot->get_status(robot);
        rt_mutex_release(&mutexRobot);
    if (status == STATUS_OK) {
        type_msg = message->get_type(movement);
        if (type_msg == MESSAGE_TYPE_MOVEMENT){
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
            switch (move->get_direction(move)) {
                case DIRECTION_FORWARD:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_LEFT:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
                case DIRECTION_RIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_STOP:
                    gauche = MOTEUR_STOP;
                    droite = MOTEUR_STOP;
                    break;
                case DIRECTION_STRAIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
            }
            vitesse_mouvement = get_speed(move);
            // regarde si la vitesse demandés pour le robot est supérieure à 50
            if (vitesse_mouvement >= 50){
                switch (move->get_direction(move))
                case DIRECTION_FORWARD:
                    gauche = MOTEUR_AVANT_RAPIDE;
                    droite = MOTEUR_AVANT_RAPIDE;
                    break;
                case DIRECTION_BACKWARD:
                    gauche = MOTEUR_ARRIERE_RAPIDE;
                    droite = MOTEUR_ARRIERE_RAPIDE;
                    break;
            }
            rt_mutex_release(&mutexMove);

	    rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	    status = robot->set_motors(robot, gauche, droite);
	    rt_mutex_release(&mutexRobot);
        }
        else if (type_msg == MESSAGE_TYPE_POSITION){
            DPosition *position;
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
	    rt_mutex_acquire(&mutexPosition, TM_INFINITE);
            put(movement, position);
            angle = position->get_orientation(position);
            rt_mutex_release(&mutexMove);
	    rt_mutex_release(&mutexPosition);
	    rt_mutex_acquire(&mutexRobot, TM_INFINITE);
            status = robot->turn(robot, (180.0*angle)/3.14, HORAIRE);
	    rt_mutex_release(&mutexRobot);
        }
	}
	if (status != STATUS_OK) {
	rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	status = robot->get_status(robot);
	rt_mutex_release(&mutexRobot);
	message = d_new_message();
        message->put_state(message, status);
        rt_printf("tmove : Envoi message\n");
        if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
	message->free(message);
	}
     }
   }
}
*/

//==========================================================================

void traiter_image (void *arg) {
   int status = 1;
   DCamera * camera = d_new_camera();
   DImage *image = d_new_image();
   DJpegimage *jpeg = d_new_jpegimage();
   DMessage *message;

   camera->open(camera);
   rt_mutex_acquire(&mutexTransmit, TM_INFINITE);
   img_transmit = 1;
   rt_mutex_release(&mutexTransmit);
  

    rt_printf("ttraiter_image : Debut de l'éxecution de periodique à 600ms\n");
    rt_task_set_periodic(NULL, TM_NOW, 600000000);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_printf("ttraiter_image : Activation périodique\n");

        rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
        status = etatCommMoniteur;
        rt_mutex_release(&mutexEtatMon);
	
        if (status == STATUS_OK) {
	  rt_mutex_acquire(&mutexTransmit, TM_INFINITE);	
	  if (img_transmit == 1) {
			 
	    camera->get_frame(camera,image);
	    jpeg->compress(jpeg,image);

	    message = d_new_message();
	    message->put_jpeg_image(message,jpeg);
	
	    rt_printf("ttraiter_image : Envoi message\n");
	    if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
	      message->free(message);
	    }
	    image->release(image);
	    jpeg->release(jpeg);
	
	  }
	  rt_mutex_release(&mutexTransmit);
	}
    }
}

//==========================================================================

int write_in_queue(RT_QUEUE *msgQueue, void * data, int size) {
    void *msg;
    int err;

    msg = rt_queue_alloc(msgQueue, size);
    memcpy(msg, &data, size);

    if ((err = rt_queue_send(msgQueue, msg, sizeof (DMessage), Q_NORMAL)) < 0) {
      rt_printf("Error msg queue send: %s\n", strerror(-err));
    }
    rt_queue_free(&queueMsgGUI, msg);

    return err;
}

//==========================================================================

void battery_level(void *arg){
	int status = 1;
	int vbat = -1;
	DMessage *message;
	
    rt_printf("tbattery_level : Debut de l'éxecution de periodique à 250ms\n");
    rt_task_set_periodic(NULL, TM_NOW, 250000000);
    while (1) {
        // Attente de l'activation périodique 
        rt_task_wait_period(NULL);
        rt_printf("tbattery_level : Activation périodique\n");

	rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	status = robot->get_status(robot);
	rt_mutex_release(&mutexRobot);

		
	if (status == STATUS_OK){
	  rt_mutex_acquire(&mutexBatterie, TM_INFINITE);
	  rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	  status = d_robot_get_vbat(robot, &vbat);
	  rt_mutex_release(&mutexRobot);
	  d_battery_set_level(battery, vbat);
	  d_message_put_battery_level(message, battery);
	  rt_mutex_release(&mutexBatterie);

	  rt_printf("tbattery_level : Envoi message\n");
	  if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
	    message->free(message);
	  }
		}
	}
}

//==========================================================================

// nécessite le fonctionnement de traiter_ordre

/*void detecter_arene (void * arg) {
  DCamera  *camera ;
  DImage *image ;
  DJpegimage *jpeg ;
  while (1) {
    rt_printf("tdetecter_arene : Attente de demande de detection d'arene\n");
    if (d_message_get_data(message) == ACTION_FIND_ARENA) {
      rt_mutex_acquire(&mutexTransmit, TM_INFINITE);
      img_transmit = 0 ;
      rt_mutex_release(&mutexTransmit);
      camera->_get_frame(camera,image) ;
      rt_mutex_acquire(&mutexArene, TM_INFINITE);
      arena = image->compute_arena_position(image) ;      
      d_imageshop_draw_arena(image,arena) ;
      rt_mutex_release(&mutexArene);
      jpeg->compress(jpeg,image) ;
      message->put_jpeg_image(message,jpeg) ;
      rt_printf("ttraiter_image : Envoi message\n");
      //Besoin pour ce cas d'un seul envoi ?
      //if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
	//message->free(message);
	//}
      image->release(image);
      jpeg->release(jpeg);	
    }
  }
}
*/
// CALCUL DE LA POSITION DU ROBOT 

// nécessite la réception de l'ordre du moniteur pour ACTION_COMPUTE_CONTINUOUSLY_POSITION

/*
void calcul_pos(DImage *image){
	DAction *action;
	DMessage *message;
	DJpegimage *jpeg;
	
	rt_printf("tcalcul_pos : Debut de l'éxecution de periodique à 600ms\n");
    rt_task_set_periodic(NULL, TM_NOW, 600000000);
    
    while(1){
        // Attente de l'activation périodique 
        rt_task_wait_period(NULL);
        rt_printf("tcalcul_pos : Activation périodique\n");
    	
	rt_mutex_acquire(&mutexPosition, TM_INFINITE);
	rt_mutex_acquire(&mutexArene, TM_INFINITE);
	position = d_image_compute_robot_position(image, arena);
	rt_mutex_release(&mutexArene);
    	d_imageshop_draw_position(image, position);
    	d_jpegimage_compress(jpeg, image);
    	message = d_new_message();
    	d_message_put_jpeg_image(message, jpeg);
    	// voir comment se passe l'envoi de message, si besoin de libérer le message avec free ou non
    	message = d_new_message();
    	d_message_put_position(message, position);
	rt_mutex_release(&mutexPosition);
    }
}

void fermeture_connexion_robot (void *arg) {
  int msg_arrive = 1 ;
  int status = 1 ;
  int nbre_connexions_echouees = 0 ;
  DMessage *message = d_new_message();

  while (1) {

    rt_printf("tserver : Attente d'un message\n");
    rt_mutex_acquire(&mutexServeur, TM_INFINITE);
    msg_arrive = serveur->receive(serveur, message);
    rt_mutex_release(&mutexServeur);

    if (msg_arrive > 0) {
    
    	rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
        status = etatCommMoniteur;
        rt_mutex_release(&mutexEtatMon);
      if (status != STATUS_OK) {
	if (nbre_connexion_echouees < 3)
	  nbre_connexions_echouees++ ;
	else if (nbre_connexion_echouees >= 3) {
	  d_message_put_string(message, "Connexion perdue");
	  rt_mutex_acquire(&mutexRobot, TM_INFINITE);
	  d_robot_close_com(robot);
	  rt_mutex_release(&mutexRobot);
	  nbre_connexions_echouees = 0 ;
	}
      }
      else nbre_connexions_echouees = 0 ;
    }
  }
  
  void Fermeture_connexion(void *arg) {

    int statut_serveur ;
    rt_printf("Fermeture_connexion : Attente de l'instruction de deconnexion\n");
    rt_sem_p(&semConnexionEchouee, TM_INFINITE);

    rt_mutex_acquire(&mutexServeur, TM_INFINITE);
    serveur->close(serveur) ;
    //serveur->free(serveur) ;
    rt_mutex_release(&mutexServeur) ;


    rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
    etatCommMoniteur = -1;
    rt_mutex_release(&mutexEtatMon) ;

    rt_mutex_acquire(&mutexRobot, TM_INFINITE);
    robot->close_com(robot) ;
    //robot->free(robot) ;
    rt_mutex_release(&mutexRobot) ;


}

  
  
  
  
  
}*/
