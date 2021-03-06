#include "includes.h"
#include "global.h"
#include "fonctions.h"

/**
 * \fn void initStruct(void)
 * \brief Initialisation des structures de l'application (tâches, mutex, 
 * semaphore, etc.)
 */
void initStruct(void);

/**
 * \fn void startTasks(void)
 * \brief Démarrage des tâches
 */
void startTasks(void);

/**
 * \fn void deleteTasks(void)
 * \brief Arrêt des tâches
 */
void deleteTasks(void);

int main(int argc, char**argv) {
    printf("#################################\n");
    printf("#      DE STIJL PROJECT         #\n");
    printf("#################################\n");

    //signal(SIGTERM, catch_signal);
    //signal(SIGINT, catch_signal);

    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT | MCL_FUTURE);
    /* For printing, please use rt_print_auto_init() and rt_printf () in rtdk.h
     * (The Real-Time printing library). rt_printf() is the same as printf()
     * except that it does not leave the primary mode, meaning that it is a
     * cheaper, faster version of printf() that avoids the use of system calls
     * and locks, instead using a local ring buffer per real-time thread along
     * with a process-based non-RT thread that periodically forwards the
     * contents to the output stream. main() must call rt_print_auto_init(1)
     * before any calls to rt_printf(). If you forget this part, you won't see
     * anything printed.
     */
    rt_print_auto_init(1);
    initStruct();
    startTasks();
    pause();
    deleteTasks();

    return 0;
}

void initStruct(void) {
    int err;
    /* Creation des mutex */
    if (err = rt_mutex_create(&mutexServeur, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexBatterie, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexPosition, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexTransmit, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    if (err = rt_mutex_create(&mutexEtat, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    

    if (err = rt_mutex_create(&mutexRobot, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexMove, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexArene, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    /* Creation du semaphore */
    if (err = rt_sem_create(&semConnecterRobot, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

 	if (err = rt_sem_create(&semConnexionEtablie, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
     if (err = rt_sem_create(&semConnexionEchouee, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
     if (err = rt_sem_create(&semLancerPosition, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
     if (err = rt_sem_create(&semTrouverArene, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }


    /* Creation des taches */
    if (err = rt_task_create(&tServeur, NULL, 0, PRIORITY_TSERVEUR, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tconnect, NULL, 0, PRIORITY_TCONNECT, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    if (err = rt_task_create(&tconnecter_moniteur, NULL, 0, PRIORITY_TCONNECT, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
/*
    if (err = rt_task_create(&ttraiter_ordre, NULL, 0, PRIORITY_TCONNECT, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }*/

    if (err = rt_task_create(&tmove, NULL, 0, PRIORITY_TMOVE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tenvoyer, NULL, 0, PRIORITY_TENVOYER, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    if (err = rt_task_create(&ttraiter_image, NULL, 0, PRIORITY_TIMAGE, 0)) 	{
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
	
	if (err = rt_task_create(&tbattery_level, NULL, 0, PRIORITY_TBATTERY, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    if (err = rt_task_create(&tdetecter_arene, NULL, 0, PRIORITY_TARENE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
	}

    /*if (err = rt_task_create(&tfermeture_connexion_robot, NULL, 0, PRIORITY_TFERMER_ROBOT, 0)) {
      rt_printf("Error task create: %s\n", strerror(-err));
      exit(EXIT_FAILURE);
      }*/
      
      /*if (err = rt_task_create(&tcalcul_pos, NULL, 0, PRIORITY_TPOSITION, 0)) {
      rt_printf("Error task create: %s\n", strerror(-err));
      exit(EXIT_FAILURE);
      }*/
    
    

    /* Creation des files de messages */
    if (err = rt_queue_create(&queueMsgGUI, "toto", MSG_QUEUE_SIZE*sizeof(DMessage), MSG_QUEUE_SIZE, Q_FIFO)){
        rt_printf("Error msg queue create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    /* Creation des structures globales du projet */
    robot = d_new_robot();
    move = d_new_movement();
    serveur = d_new_server();
    arena = d_new_arena();
	image = d_new_image();
    battery = d_new_battery();
}

void startTasks() {
    int err;

    if (err = rt_task_start(&tconnect, &connecter, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }


    
     if (err = rt_task_start(&tconnecter_moniteur, &connecter_moniteur, NULL)) {
        rt_printf("Error task start connecter_moniteur: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
/*
    if (err = rt_task_start(&ttraiter_ordre, &traiter_ordre, NULL)) {
        rt_printf("Error task start traiter_ordre: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
*/
    if (err = rt_task_start(&tServeur, &communiquer, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tmove, &deplacer, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tenvoyer, &envoyer, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
	
    if (err = rt_task_start(&ttraiter_image, &traiter_image, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

     
    if (err = rt_task_start(&tbattery_level, &battery_level, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

	// à rajouter dès que le traitement des ordres du moniteur fonctionnera
    /*
    if (err = rt_task_start(&tdetecter_arene, &detecter_arene, NULL)) {
      rt_printf("Error task start: %s\n", strerror(-err));
      exit(EXIT_FAILURE);
     }*/

     /*if (err = rt_task_start(&tfermeture_connexion_robot, &fermeture_connexion_robot, NULL)) {
      rt_printf("Error task start: %s\n", strerror(-err));
      exit(EXIT_FAILURE);
     }*/
     
     /*if (err = rt_calcul_pos(&tcalcul_pos, &calcul_pos, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }*/    

}

void deleteTasks() {
    rt_task_delete(&tServeur);
    rt_task_delete(&tconnect);
    rt_task_delete(&tmove);
    rt_task_delete(&ttraiter_image);
    rt_task_delete(&tbattery_level);
    rt_task_delete(&tdetecter_arene);
    //rt_task_delete(&tfermeture_connexion_robot);
    //rt_task_delete(&tcalcul_pos);

    rt_task_delete(&tconnecter_moniteur);
    //rt_task_delete(&ttraiter_ordre);
}
