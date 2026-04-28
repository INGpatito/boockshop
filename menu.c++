#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Clase Producto
class Producto {
private:
    int id;
    string nombre;
    double precio;
    int cantidad;
    
public:
    // Constructor por defecto
    Producto() : id(0), nombre(""), precio(0.0), cantidad(0) {}
    
    // Constructor parametrizado
    Producto(int id, string nombre, double precio, int cantidad) 
        : id(id), nombre(nombre), precio(precio), cantidad(cantidad) {}
    
    // Getters
    int getId() const { return id; }
    string getNombre() const { return nombre; }
    double getPrecio() const { return precio; }
    int getCantidad() const { return cantidad; }
    
    // Setters
    void setNombre(const string& nombre) { this->nombre = nombre; }
    void setPrecio(double precio) { this->precio = precio; }
    void setCantidad(int cantidad) { this->cantidad = cantidad; }
    
    // Calcular valor total del inventario para este producto
    double valorTotal() const {
        return precio * cantidad;
    }
    
    // Mostrar información del producto
    void mostrar() const {
        cout << left << setw(5) << id
             << setw(20) << nombre
             << setw(10) << fixed << setprecision(2) << precio
             << setw(10) << cantidad
             << setw(12) << valorTotal() << endl;
    }
};

// Clase Inventario
class Inventario {
private:
    vector<Producto> productos;
    int siguienteId;
    
public:
    Inventario() : siguienteId(1) {}
    
    // Agregar un nuevo producto
    void agregarProducto() {
        string nombre;
        double precio;
        int cantidad;
        
        cout << "\n--- AGREGAR NUEVO PRODUCTO ---\n";
        cout << "Nombre: ";
        cin.ignore();
        getline(cin, nombre);
        cout << "Precio: $";
        cin >> precio;
        cout << "Cantidad: ";
        cin >> cantidad;
        
        productos.push_back(Producto(siguienteId++, nombre, precio, cantidad));
        cout << "Producto agregado exitosamente con ID: " << siguienteId - 1 << endl;
    }
    
    // Mostrar todos los productos
    void mostrarProductos() const {
        if (productos.empty()) {
            cout << "\nNo hay productos en el inventario.\n";
            return;
        }
        
        cout << "\n--- LISTA DE PRODUCTOS ---\n";
        cout << left << setw(5) << "ID"
             << setw(20) << "Nombre"
             << setw(10) << "Precio"
             << setw(10) << "Cantidad"
             << setw(12) << "Valor Total" << endl;
        cout << string(57, '-') << endl;
        
        for (const auto& producto : productos) {
            producto.mostrar();
        }
        
        double totalInventario = 0;
        for (const auto& producto : productos) {
            totalInventario += producto.valorTotal();
        }
        cout << string(57, '-') << endl;
        cout << "VALOR TOTAL DEL INVENTARIO: $" << fixed << setprecision(2) << totalInventario << endl;
    }
    
    // Buscar producto por ID
    int buscarPorId(int id) const {
        for (size_t i = 0; i < productos.size(); i++) {
            if (productos[i].getId() == id) {
                return i;
            }
        }
        return -1;
    }
    
    // Modificar un producto
    void modificarProducto() {
        if (productos.empty()) {
            cout << "\nNo hay productos para modificar.\n";
            return;
        }
        
        int id;
        cout << "\n--- MODIFICAR PRODUCTO ---\n";
        cout << "Ingrese el ID del producto a modificar: ";
        cin >> id;
        
        int indice = buscarPorId(id);
        if (indice == -1) {
            cout << "Producto no encontrado.\n";
            return;
        }
        
        cout << "Producto encontrado:\n";
        productos[indice].mostrar();
        
        string nombre;
        double precio;
        int cantidad;
        char opcion;
        
        cout << "\n¿Modificar nombre? (s/n): ";
        cin >> opcion;
        if (opcion == 's' || opcion == 'S') {
            cout << "Nuevo nombre: ";
            cin.ignore();
            getline(cin, nombre);
            productos[indice].setNombre(nombre);
        }
        
        cout << "¿Modificar precio? (s/n): ";
        cin >> opcion;
        if (opcion == 's' || opcion == 'S') {
            cout << "Nuevo precio: $";
            cin >> precio;
            productos[indice].setPrecio(precio);
        }
        
        cout << "¿Modificar cantidad? (s/n): ";
        cin >> opcion;
        if (opcion == 's' || opcion == 'S') {
            cout << "Nueva cantidad: ";
            cin >> cantidad;
            productos[indice].setCantidad(cantidad);
        }
        
        cout << "Producto modificado exitosamente.\n";
    }
    
    // Eliminar un producto
    void eliminarProducto() {
        if (productos.empty()) {
            cout << "\nNo hay productos para eliminar.\n";
            return;
        }
        
        int id;
        cout << "\n--- ELIMINAR PRODUCTO ---\n";
        cout << "Ingrese el ID del producto a eliminar: ";
        cin >> id;
        
        int indice = buscarPorId(id);
        if (indice == -1) {
            cout << "Producto no encontrado.\n";
            return;
        }
        
        cout << "Producto a eliminar:\n";
        productos[indice].mostrar();
        
        char confirmacion;
        cout << "¿Está seguro de eliminar este producto? (s/n): ";
        cin >> confirmacion;
        
        if (confirmacion == 's' || confirmacion == 'S') {
            productos.erase(productos.begin() + indice);
            cout << "Producto eliminado exitosamente.\n";
        } else {
            cout << "Eliminación cancelada.\n";
        }
    }
    
    // Guardar inventario en archivo
    void guardarEnArchivo() const {
        ofstream archivo("inventario.txt");
        
        if (!archivo) {
            cout << "Error al abrir el archivo para escritura.\n";
            return;
        }
        
        for (const auto& producto : productos) {
            archivo << producto.getId() << ","
                    << producto.getNombre() << ","
                    << producto.getPrecio() << ","
                    << producto.getCantidad() << endl;
        }
        
        archivo.close();
        cout << "Inventario guardado en 'inventario.txt' exitosamente.\n";
    }
    
    // Cargar inventario desde archivo
    void cargarDesdeArchivo() {
        ifstream archivo("inventario.txt");
        
        if (!archivo) {
            cout << "No se encontró archivo de inventario previo.\n";
            return;
        }
        
        productos.clear();
        string linea;
        int maxId = 0;
        
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            
            // Parsear la línea
            size_t pos1 = linea.find(',');
            size_t pos2 = linea.find(',', pos1 + 1);
            size_t pos3 = linea.find(',', pos2 + 1);
            
            int id = stoi(linea.substr(0, pos1));
            string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
            double precio = stod(linea.substr(pos2 + 1, pos3 - pos2 - 1));
            int cantidad = stoi(linea.substr(pos3 + 1));
            
            productos.push_back(Producto(id, nombre, precio, cantidad));
            
            if (id > maxId) {
                maxId = id;
            }
        }
        
        siguienteId = maxId + 1;
        archivo.close();
        cout << "Inventario cargado exitosamente. " << productos.size() << " productos encontrados.\n";
    }
};

// Función para mostrar el menú
void mostrarMenu() {
    cout << "\n===== SISTEMA DE INVENTARIO =====\n";
    cout << "1. Agregar producto\n";
    cout << "2. Mostrar todos los productos\n";
    cout << "3. Modificar producto\n";
    cout << "4. Eliminar producto\n";
    cout << "5. Guardar inventario\n";
    cout << "6. Cargar inventario\n";
    cout << "7. Salir\n";
    cout << "Seleccione una opción: ";
}

int main() {
    Inventario inventario;
    int opcion;
    
    cout << "SISTEMA DE GESTIÓN DE INVENTARIO\n";
    cout << "===============================\n";
    
    // Intentar cargar inventario existente al iniciar
    inventario.cargarDesdeArchivo();
    
    do {
        mostrarMenu();
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                inventario.agregarProducto();
                break;
            case 2:
                inventario.mostrarProductos();
                break;
            case 3:
                inventario.modificarProducto();
                break;
            case 4:
                inventario.eliminarProducto();
                break;
            case 5:
                inventario.guardarEnArchivo();
                break;
            case 6:
                inventario.cargarDesdeArchivo();
                break;
            case 7:
                cout << "\n¿Desea guardar antes de salir? (s/n): ";
                char respuesta;
                cin >> respuesta;
                if (respuesta == 's' || respuesta == 'S') {
                    inventario.guardarEnArchivo();
                }
                cout << "¡Hasta luego!\n";
                break;
            default:
                cout << "Opción no válida. Intente de nuevo.\n";
        }
        
    } while (opcion != 7);
    
    return 0;
}
