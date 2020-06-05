// #include <pipe.h>
// #include <process.h>
//
// pipe pipeList[MAX_PIPES];
//
// void initPipes(){
//     for(int i=0; i<MAX_PIPES; i++){
//         for(int j=0;j<MAX_BUFF_SIZE;j++){
//             pipeList[i].buffer[j]='?'; //se podria cambiar el char, puse este para saber cuando llegamos al final del buffer al leerlo
//         }
//         pipeList[i].available = 1;
//         pipeList[i].fd = 0; //talvez conviene poner 2 para no dejar por default read (creo que el 0 es read)
//         pipeList[i].parentProcess = NULL;
//         pipeList[i].targetProcess = NULL;
//     }
// }
//
// int newPipe(process * target){
//     for(int i=0; i<MAX_PIPES;i++){
//         if(pipeList[i].available == 1){
//
//         }
//     }
// }
