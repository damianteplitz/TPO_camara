void main (void){
    initi2c(){
        while (1){
            maq_master();
            //por systick pido dato en data_request cada 10s (hacer)
        }
    }
}

void maq_master (){
    switch (mode){
        //quiza halla que ponerle un flag a cada estado para asegurar que esta en ese estado!!
        case ESPERANDO:
            if (data_requested()){
                send_start();
                load_SLA();
                load_R();
                clear_STA();
                if (mode_i2c() == 0x08){
                    mode = COMIENZO_TRANSMISION;
                    flag_comienzo_transmision = TRUE;
                }
                break;
            }
        case COMIENZO_TRANSMISION:                      //en el de johnyy el primer dato lo lee aca
            if (mode_i2c() == 40){ //entra aca cuando recibi el ack y empieza automaticamente la transmision
                mode = RECIBO_DATO_0;
                flag_leer = TRUE;
            }
            break;
        
        case RECIBO_DATO_0:
            if (flag_leer){
                save_data();
                flag_leer = FALSE;
            }
            if (data_recieved() == ENDED){
                send_noack();
            }else {
                send_ack();
            }
            if (mode_i2c() == 50){
                    mode = RECIBO_DATOS;
                    flag_leer = TRUE;
                }
            if (mode_i2c() == 58){
                    mode = COMUNICACION_TERMINADA;
                }
            break;
        case RECIBO_DATOS·
            if (flag_leer){
                save_data();
                flag_leer = FALSE;
            }
            if (data_recieved() == ENDED){
                send_noack();
            }else {
                send_ack();
            }
            if (mode_i2c() == 50){
                    mode = RECIBO_DATOS;
                    flag_leer = TRUE;
                }
            if (mode_i2c() == 58){
                    mode = COMUNICACION_TERMINADA;
                }
            break;
        case: COMUNICACION_TERMINADA:
            send_stop();
            mode = ESPERANDO;
            break;
    }
}

HACER INICIALIZACION SEND: STOP,START Y ACK,NOACK Y SAVE
