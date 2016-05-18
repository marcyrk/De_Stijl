#include "fonctions.h"

int write_in_queue(RT_QUEUE *msgQueue, void * data, int size);

void envoyer(void * arg) {
    DMessage *msg;
    int err;

    while (1) {
        rt_printf("tenvoyer : Attente d'un message\n");
        if ((err = rt_queue_read(&queueMsgGUI, &msg, sizeof (DMessage), TM_INFINITE)) >= 0) {
            rt_printf("tenvoyer : envoi d'un message au moniteur\n");
            serveur->send(serveur, msg);
            msg->free(msg);
        } else {
            rt_printf("Error msg queue write: %s\n", strerror(-err));
        }
    }
}

void connecter(void * arg) {
    int status;
    DMessage *message;

    rt_printf("tconnect : Debut de l'exécution de tconnect\n");

    while (1) {
        rt_printf("tconnect : Attente du sémarphore semConnecterRobot\n");
        rt_sem_p(&semConnecterRobot, TM_INFINITE);
        rt_printf("tconnect : Ouverture de la communication avec le robot\n");
        status = robot->open_device(robot);

        rt_mutex_acquire(&mutexEtatRob, TM_INFINITE);
        etatCommRobot = status;
        rt_mutex_release(&mutexEtatRob);

        if (status == STATUS_OK) {
            status = robot->start_insecurely(robot);
            if (status == STATUS_OK){
                rt_printf("tconnect : Robot démarrer\n");
            }
        }

        message = d_new_message();
        message->put_state(message, status);

        rt_printf("tconnecter : Envoi message\n");
        message->print(message, 100);

        if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
            message->free(message);
        }
    }
}

void communiquer(void *arg) {
    DMessage *msg = d_new_message();
    int var1 = 1;
    int num_msg = 0;

    rt_printf("tserver : Début de l'exécution de serveur\n");
    serveur->open(serveur, "8000");
    rt_printf("tserver : Connexion\n");

    rt_mutex_acquire(&mutexEtatMon, TM_INFINITE);
    etatCommMoniteur = 0;
    rt_mutex_release(&mutexEtatMon);

    while (var1 > 0) {
        rt_printf("tserver : Attente d'un message\n");
        var1 = serveur->receive(serveur, msg);
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
                            rt_sem_v(&semConnecterRobot);
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

        rt_mutex_acquire(&mutexEtatRob, TM_INFINITE);
        status = etatCommRobot;
        rt_mutex_release(&mutexEtatRob);

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

            status = robot->set_motors(robot, gauche, droite);

            if (status != STATUS_OK) {
                rt_mutex_acquire(&mutexEtatRob, TM_INFINITE);
                etatCommRobot = status;
                rt_mutex_release(&mutexEtatRob);

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


void traiter_image (void *arg) {
   int status = 1;
   DCamera * camera = d_new_camera();
   DImage *image = d_new_image();
   DJpegimage *jpeg = d_new_jpegimage();
   DMessage *message;

    camera->open(camera);
    img_transmit = 1;

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
		}
    }
}

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
/*
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
        
		rt_mutex_acquire(&mutexEtatRob, TM_INFINITE);
		status = etatCommRobot;
		rt_mutex_release(&mutexEtatRob);
		
		if (status == STATUS_OK){
			status = d_robot_get_vbat(robot, &vbat);
			d_battery_set_level(battery, vbat);
			d_message_put_battery_level(message, battery);
		}
	}
}
*/
/*void detecter_arene (void * arg) {

  DCamera  *camera ;
  DImage *image ;
  DJpegimage *jpeg ;

  while (1) {
    rt_printf("tdetecter_arene : Attente de demande de detection d'arene\n");

    if (d_message_get_data(message) == ACTION_FIND_ARENA) {
      img_transmit = 0 ;
      camera->_get_frame(camera,image) ;
      rt_mutex_acquire(&mutexArene, TM_INFINITE);
      arena = image->compute_arena_position(image) ;
      rt_mutex_release(&mutexArene);
      d_imageshop_draw_arena(image,arena) ;
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
    	
    	position = d_image_compute_robot_position(image, arena);
    	d_imageshop_draw_position(image, position);
    	d_jpegimage_compress(jpeg, image);
    	message = d_new_message();
    	d_message_put_jpeg_image(message, jpeg);
    	// voir comment se passe l'envoi de message, si besoin de libérer le message avec free ou non
    	message = d_new_message();
    	d_message_put_position(message, position);
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
	  d_robot_close_com(robot);
	  nbre_connexions_echouees = 0 ;
	}
      }
      else nbre_connexions_echouees = 0 ;
    }
  }
}*/


