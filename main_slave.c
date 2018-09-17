void main (void){
    initi2c(){
        while (1){
            maq_slave();
        }
    }
}

void maq_slave (){
    switch (mode){
        case ENVIAR:
            if (i2c_state = 0xA8){
                write (BUF);
                BUF++;
                case = ESPERAR;
            }
            break;
        case ESPERAR:
            if (i2c_state = 0xB8){
                write(BUF);
            }
            if (i2c_state = 0xC0){
                case = ENVIAR;
            }
            break;
    }
}

HACER WRITE
