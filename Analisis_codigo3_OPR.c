//Analisis de código 3 OPR


#include <stdio.h>   // Libreria estandar de entrada y salida
#include <stdlib.h>  // Libreria con funciones como malloc y free
#include <stdbool.h> // Libreria para manejar el tipo booleano

#define NUM_FRAMES 4   // Define el numero de frames (paginas fisicas en memoria)
#define NUM_PAGES 10   // Define el numero total de paginas virtuales

// Estructura para representar un frame en la memoria fisica
typedef struct Frame {
    int page;           // Numero de pagina almacenada en el frame (-1 si esta vacio)
    bool valid;         // Indica si el frame esta ocupado (true) o vacio (false)
    struct Frame *prev; // Puntero al frame anterior (para lista doblemente enlazada)
    struct Frame *next; // Puntero al siguiente frame (para lista doblemente enlazada)
} Frame;

// Estructura para manejar la lista de frames en memoria fisica
typedef struct FrameList {
    int numFrames;      // Numero de frames actualmente ocupados
    Frame *head;        // Puntero al primer frame de la lista
    Frame *tail;        // Puntero al ultimo frame de la lista
} FrameList;

// Funcion para crear un nuevo frame
Frame* createFrame() {
    Frame *frame = (Frame *)malloc(sizeof(Frame));  // Reserva memoria para el frame
    if (frame != NULL) {  // Verifica si la memoria fue asignada correctamente
        frame->page = -1;   // Inicialmente no hay pagina asignada
        frame->valid = false;  // Indica que el frame esta vacio
        frame->prev = NULL;    // Inicializa el puntero previo como NULL
        frame->next = NULL;    // Inicializa el puntero siguiente como NULL
    }
    return frame;  // Retorna el frame creado
}

// Funcion para inicializar la lista de frames en memoria fisica
FrameList* createFrameList() {
    FrameList *frameList = (FrameList *)malloc(sizeof(FrameList));  // Reserva memoria para la lista de frames
    if (frameList != NULL) {  // Verifica si la memoria fue asignada correctamente
        frameList->numFrames = 0;  // Inicializa el contador de frames ocupados en 0
        frameList->head = NULL;    // Inicializa el puntero al primer frame como NULL
        frameList->tail = NULL;    // Inicializa el puntero al ultimo frame como NULL
    }
    return frameList;  // Retorna la lista de frames creada
}

// Funcion para insertar un frame al frente de la lista (mas recientemente usado)
void insertFrame(FrameList *frameList, Frame *frame) {
    if (frameList->head == NULL) {  // Si la lista esta vacia
        frameList->head = frame;  // El frame se convierte en el primero y ultimo de la lista
        frameList->tail = frame;
    } else {
        // Inserta el frame al frente de la lista
        frame->next = frameList->head;   // El nuevo frame apunta al actual primer frame
        frameList->head->prev = frame;   // El actual primer frame apunta al nuevo como previo
        frameList->head = frame;         // El nuevo frame se convierte en el primero
    }
    frameList->numFrames++;  // Incrementa el numero de frames ocupados
}

// Funcion para eliminar un frame de la lista (menos recientemente usado)
void removeFrame(FrameList *frameList, Frame *frame) {
    if (frame->prev != NULL) {
        frame->prev->next = frame->next;  // Actualiza el puntero siguiente del frame previo
    } else {
        frameList->head = frame->next;    // Si es el primer frame, actualiza el inicio de la lista
    }
    if (frame->next != NULL) {
        frame->next->prev = frame->prev;  // Actualiza el puntero previo del frame siguiente
    } else {
        frameList->tail = frame->prev;    // Si es el ultimo frame, actualiza el final de la lista
    }
    frameList->numFrames--;  // Decrementa el numero de frames ocupados
    free(frame);  // Libera la memoria del frame eliminado
}

// Funcion para buscar un frame por el numero de pagina
Frame* findFrame(FrameList *frameList, int page) {
    Frame *current = frameList->head;  // Comienza desde el primer frame
    while (current != NULL) {  // Recorre la lista
        if (current->page == page) {   // Si encuentra el frame con la pagina buscada
            return current;  // Retorna el frame encontrado
        }
        current = current->next;  // Pasa al siguiente frame en la lista
    }
    return NULL;  // Retorna NULL si no encuentra el frame
}

// Funcion para simular la carga de una pagina a memoria fisica usando el Algoritmo Optimo
void loadPage(FrameList *frameList, int page, int futureAccess[]) {
    Frame *frame = createFrame();  // Crea un nuevo frame para la pagina
    frame->page = page;  // Asigna la pagina al frame
    frame->valid = true; // Marca el frame como ocupado
    
    // Si la lista de frames ya esta llena, determinar el reemplazo optimo
    if (frameList->numFrames == NUM_FRAMES) {
        Frame *optimalFrame = NULL;  // Inicializa el frame optimo como NULL
        int farthest = -1;  // Variable para registrar la pagina mas lejana en uso futuro
        
        // Buscar la pagina que no sera usada por mas tiempo
        for (int i = 0; i < NUM_FRAMES; ++i) {
            Frame *current = frameList->head;
            int j = 0;
            while (current != NULL && current->page != -1 && current->page != futureAccess[j]) {
                current = current->next;
                ++j;
            }
            if (current == NULL || current->page == -1) {
                optimalFrame = current;  // Si encuentra una pagina no usada en el futuro
                break;
            }
            if (j > farthest) {
                farthest = j;  // Actualiza la pagina que se usara mas tarde
                optimalFrame = current;
            }
        }
        
        // Remueve el frame con la pagina optima encontrada
        if (optimalFrame != NULL) {
            removeFrame(frameList, optimalFrame);  // Elimina el frame optimo
        }
    }
    
    // Inserta el nuevo frame en la lista de frames
    insertFrame(frameList, frame);
}

// Funcion para imprimir el estado actual de la lista de frames (para depuracion)
void printFrameList(FrameList *frameList) {
    printf("Estado actual de la lista de frames:\n");  // Mensaje de inicio
    Frame *current = frameList->head;  // Comienza desde el primer frame
    while (current != NULL) {  // Recorre la lista
        printf("Pagina: %d, ", current->page);  // Imprime el numero de pagina
        if (current->valid) {
            printf("Estado: Ocupado\n");  // Imprime si el frame esta ocupado
        } else {
            printf("Estado: Vacio\n");    // Imprime si el frame esta vacio
        }
        current = current->next;  // Pasa al siguiente frame en la lista
    }
    printf("\n");  // Salto de linea al final
}

int main() {
    FrameList *frameList = createFrameList();  // Crea la lista de frames
    
    // Simula el acceso futuro de paginas
    int futureAccess[NUM_PAGES] = {1, 2, 3, 4, 5, 1, 2, 1, 3, 4};
    
    // Simula la carga de paginas a la memoria fisica usando el Algoritmo Optimo
    for (int i = 0; i < NUM_PAGES; ++i) {
        loadPage(frameList, futureAccess[i], futureAccess + i + 1);
        printFrameList(frameList);  // Imprime el estado despues de cada carga
    }
    
    // Libera la memoria utilizada por la lista de frames
    free(frameList);
    
    return 0; //RETURN GOD
}

