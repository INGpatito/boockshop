#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define FILE_NAME "inventario.dat"

// Estructura para un producto
typedef struct {
    int id;
    char nombre[MAX_NAME];
    int cantidad;
    float precio;
} Producto;

// Prototipos de funciones
void agregarProducto();
void mostrarInventario();
void buscarProducto();
void modificarProducto();
void eliminarProducto();
void guardarProducto(Producto p);
int cargarProductos(Producto productos[], int max);
int generarNuevoID();

int main() {
    int opcion;

    do {
        printf("\n===== SISTEMA DE GESTION DE INVENTARIO =====\n");
        printf("1. Agregar producto\n");
        printf("2. Mostrar inventario\n");
        printf("3. Buscar producto\n");
        printf("4. Modificar producto\n");
        printf("5. Eliminar producto\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada invalida. Intente de nuevo.\n");
            while (getchar() != '\n'); // limpiar buffer
            continue;
        }

        switch (opcion) {
            case 1: agregarProducto(); break;
            case 2: mostrarInventario(); break;
            case 3: buscarProducto(); break;
            case 4: modificarProducto(); break;
            case 5: eliminarProducto(); break;
            case 0: printf("Saliendo...\n"); break;
            default: printf("Opcion invalida.\n");
        }
    } while (opcion != 0);

    return 0;
}

// Genera un nuevo ID basado en el último guardado
int generarNuevoID() {
    FILE *f = fopen(FILE_NAME, "rb");
    Producto p;
    int ultimoID = 0;

    if (f) {
        while (fread(&p, sizeof(Producto), 1, f)) {
            if (p.id > ultimoID) ultimoID = p.id;
        }
        fclose(f);
    }
    return ultimoID + 1;
}

// Guarda un producto en el archivo
void guardarProducto(Producto p) {
    FILE *f = fopen(FILE_NAME, "ab");
    if (!f) {
        printf("Error al abrir el archivo.\n");
        return;
    }
    fwrite(&p, sizeof(Producto), 1, f);
    fclose(f);
}

// Carga productos en un arreglo
int cargarProductos(Producto productos[], int max) {
    FILE *f = fopen(FILE_NAME, "rb");
    int count = 0;
    if (!f) return 0;

    while (fread(&productos[count], sizeof(Producto), 1, f) && count < max) {
        count++;
    }
    fclose(f);
    return count;
}

// Agregar producto
void agregarProducto() {
    Producto p;
    p.id = generarNuevoID();

    printf("Ingrese nombre del producto: ");
    while (getchar() != '\n'); // limpiar buffer
    fgets(p.nombre, MAX_NAME, stdin);
    p.nombre[strcspn(p.nombre, "\n")] = '\0'; // quitar salto de línea

    printf("Ingrese cantidad: ");
    if (scanf("%d", &p.cantidad) != 1 || p.cantidad < 0) {
        printf("Cantidad invalida.\n");
        return;
    }

    printf("Ingrese precio: ");
    if (scanf("%f", &p.precio) != 1 || p.precio < 0) {
        printf("Precio invalido.\n");
        return;
    }

    guardarProducto(p);
    printf("Producto agregado con ID %d.\n", p.id);
}

// Mostrar inventario
void mostrarInventario() {
    Producto productos[100];
    int total = cargarProductos(productos, 100);

    if (total == 0) {
        printf("Inventario vacio.\n");
        return;
    }

    printf("\n%-5s %-20s %-10s %-10s\n", "ID", "Nombre", "Cantidad", "Precio");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < total; i++) {
        printf("%-5d %-20s %-10d $%-10.2f\n",
               productos[i].id, productos[i].nombre,
               productos[i].cantidad, productos[i].precio);
    }
}

// Buscar producto por ID
void buscarProducto() {
    int id;
    printf("Ingrese ID del producto a buscar: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada invalida.\n");
        return;
    }

    Producto p;
    FILE *f = fopen(FILE_NAME, "rb");
    if (!f) {
        printf("No hay inventario.\n");
        return;
    }

    int encontrado = 0;
    while (fread(&p, sizeof(Producto), 1, f)) {
        if (p.id == id) {
            printf("Producto encontrado: %s, Cantidad: %d, Precio: %.2f\n",
                   p.nombre, p.cantidad, p.precio);
            encontrado = 1;
            break;
        }
    }
    fclose(f);

    if (!encontrado) printf("Producto no encontrado.\n");
}

// Modificar producto
void modificarProducto() {
    int id;
    printf("Ingrese ID del producto a modificar: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada invalida.\n");
        return;
    }

    Producto productos[100];
    int total = cargarProductos(productos, 100);
    int encontrado = 0;

    for (int i = 0; i < total; i++) {
        if (productos[i].id == id) {
            encontrado = 1;
            printf("Nuevo nombre (actual: %s): ", productos[i].nombre);
            while (getchar() != '\n');
            fgets(productos[i].nombre, MAX_NAME, stdin);
            productos[i].nombre[strcspn(productos[i].nombre, "\n")] = '\0';

            printf("Nueva cantidad (actual: %d): ", productos[i].cantidad);
            scanf("%d", &productos[i].cantidad);

            printf("Nuevo precio (actual: %.2f): ", productos[i].precio);
            scanf("%f", &productos[i].precio);
            break;
        }
    }

    if (!encontrado) {
        printf("Producto no encontrado.\n");
        return;
    }

    FILE *f = fopen(FILE_NAME, "wb");
    if (!f) {
        printf("Error al guardar cambios.\n");
        return;
    }
    fwrite(productos, sizeof(Producto), total, f);
    fclose(f);
    printf("Producto modificado correctamente.\n");
}

// Eliminar producto
void eliminarProducto() {
    int id;
    printf("Ingrese ID del producto a eliminar: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada invalida.\n");
        return;
    }

    Producto productos[100];
    int total = cargarProductos(productos, 
