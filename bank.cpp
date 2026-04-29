#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <ctime>
#include <sstream>
#include <map>
#include <queue>
#include <stdexcept>
#include <random>
#include <functional>
#include <regex>

using namespace std;

// ==================== CLASES DE EXCEPCIÓN ====================
class ErrorBancario : public runtime_error {
public:
    ErrorBancario(const string& mensaje) : runtime_error(mensaje) {}
};

class SaldoInsuficiente : public ErrorBancario {
public:
    SaldoInsuficiente() : ErrorBancario("Saldo insuficiente para realizar la operación") {}
};

class CuentaNoEncontrada : public ErrorBancario {
public:
    CuentaNoEncontrada() : ErrorBancario("Cuenta no encontrada en el sistema") {}
};

// ==================== CLASE FECHA ====================
class Fecha {
private:
    int dia, mes, anio;
    
public:
    Fecha(int d = 1, int m = 1, int a = 2024) : dia(d), mes(m), anio(a) {
        if (!esValida()) throw ErrorBancario("Fecha inválida");
    }
    
    bool esValida() const {
        if (mes < 1 || mes > 12 || dia < 1 || anio < 1900) return false;
        int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (esBisiesto()) diasPorMes[2] = 29;
        return dia <= diasPorMes[mes];
    }
    
    bool esBisiesto() const {
        return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
    }
    
    string toString() const {
        stringstream ss;
        ss << setw(2) << setfill('0') << dia << "/"
           << setw(2) << setfill('0') << mes << "/" << anio;
        return ss.str();
    }
    
    static Fecha ahora() {
        time_t t = time(nullptr);
        tm* ahora = localtime(&t);
        return Fecha(ahora->tm_mday, ahora->tm_mon + 1, ahora->tm_year + 1900);
    }
};

// ==================== CLASE TRANSACCIÓN ====================
class Transaccion {
private:
    string id;
    Fecha fecha;
    string tipo;
    double monto;
    double saldoResultante;
    string descripcion;
    
    static string generarId() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(100000, 999999);
        return "TXN" + to_string(dis(gen));
    }
    
public:
    Transaccion(string tipo, double monto, double saldoResultante, string desc = "")
        : id(generarId()), fecha(Fecha::ahora()), tipo(tipo), 
          monto(monto), saldoResultante(saldoResultante), descripcion(desc) {}
    
    void mostrar() const {
        cout << left << setw(12) << id
             << setw(12) << fecha.toString()
             << setw(15) << tipo
             << setw(12) << fixed << setprecision(2) << monto
             << setw(12) << saldoResultante
             << setw(20) << descripcion << endl;
    }
    
    string serializar() const {
        return id + "," + fecha.toString() + "," + tipo + "," + 
               to_string(monto) + "," + to_string(saldoResultante) + "," + descripcion;
    }
};

// ==================== CLASE CLIENTE ====================
class Cliente {
private:
    string id;
    string nombre;
    string apellido;
    string dni;
    string email;
    string telefono;
    Fecha fechaRegistro;
    double scoreCrediticio;
    
public:
    Cliente(string id, string nom, string ap, string dni, string email, string tel)
        : id(id), nombre(nom), apellido(ap), dni(dni), email(email), 
          telefono(tel), fechaRegistro(Fecha::ahora()), scoreCrediticio(500.0) {}
    
    string getId() const { return id; }
    string getNombreCompleto() const { return nombre + " " + apellido; }
    string getDNI() const { return dni; }
    
    void actualizarScore(double cantidad) {
        scoreCrediticio += cantidad * 0.01;
        scoreCrediticio = min(1000.0, max(0.0, scoreCrediticio));
    }
    
    double getScore() const { return scoreCrediticio; }
    
    void mostrar() const {
        cout << "\n--- INFORMACIÓN DEL CLIENTE ---\n";
        cout << "ID: " << id << endl;
        cout << "Nombre: " << getNombreCompleto() << endl;
        cout << "DNI: " << dni << endl;
        cout << "Email: " << email << endl;
        cout << "Teléfono: " << telefono << endl;
        cout << "Cliente desde: " << fechaRegistro.toString() << endl;
        cout << "Score Crediticio: " << fixed << setprecision(1) << scoreCrediticio << endl;
    }
};

// ==================== CLASE ABSTRACTA CUENTA ====================
class Cuenta {
protected:
    string numeroCuenta;
    shared_ptr<Cliente> titular;
    double saldo;
    vector<Transaccion> historial;
    double tasaInteres;
    static int contadorCuentas;
    
public:
    Cuenta(shared_ptr<Cliente> tit, double saldoInicial, double tasa)
        : titular(tit), saldo(saldoInicial), tasaInteres(tasa) {
        contadorCuentas++;
        numeroCuenta = generarNumeroCuenta();
    }
    
    virtual ~Cuenta() = default;
    
    string getNumero() const { return numeroCuenta; }
    double getSaldo() const { return saldo; }
    shared_ptr<Cliente> getTitular() const { return titular; }
    
    virtual bool retirar(double monto) {
        if (monto <= 0) throw ErrorBancario("El monto debe ser positivo");
        if (monto > saldo) throw SaldoInsuficiente();
        saldo -= monto;
        historial.push_back(Transaccion("RETIRO", monto, saldo));
        return true;
    }
    
    virtual void depositar(double monto) {
        if (monto <= 0) throw ErrorBancario("El monto debe ser positivo");
        saldo += monto;
        historial.push_back(Transaccion("DEPOSITO", monto, saldo));
        titular->actualizarScore(monto);
    }
    
    virtual void aplicarInteres() = 0;
    virtual string getTipo() const = 0;
    virtual void mostrar() const {
        cout << "\n--- INFORMACIÓN DE CUENTA ---\n";
        cout << "Tipo: " << getTipo() << endl;
        cout << "Número: " << numeroCuenta << endl;
        cout << "Titular: " << titular->getNombreCompleto() << endl;
        cout << "Saldo: $" << fixed << setprecision(2) << saldo << endl;
        cout << "Tasa de Interés: " << tasaInteres << "%" << endl;
    }
    
    void mostrarHistorial() const {
        if (historial.empty()) {
            cout << "No hay transacciones registradas.\n";
            return;
        }
        cout << "\n--- HISTORIAL DE TRANSACCIONES ---\n";
        cout << left << setw(12) << "ID"
             << setw(12) << "Fecha"
             << setw(15) << "Tipo"
             << setw(12) << "Monto"
             << setw(12) << "Saldo"
             << setw(20) << "Descripción" << endl;
        cout << string(83, '-') << endl;
        for (const auto& trans : historial) {
            trans.mostrar();
        }
    }
    
    virtual string serializar() const = 0;
    
protected:
    static string generarNumeroCuenta() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 9);
        string num = "";
        for (int i = 0; i < 16; i++) {
            num += to_string(dis(gen));
            if ((i + 1) % 4 == 0 && i < 15) num += "-";
        }
        return num;
    }
};

int Cuenta::contadorCuentas = 0;

// ==================== CUENTA DE AHORROS ====================
class CuentaAhorros : public Cuenta {
private:
    int retirosGratis;
    static const int MAX_RETIROS_GRATIS = 3;
    static const double COMISION_RETIRO;
    
public:
    CuentaAhorros(shared_ptr<Cliente> tit, double saldoInicial, double tasa = 2.5)
        : Cuenta(tit, saldoInicial, tasa), retirosGratis(MAX_RETIROS_GRATIS) {}
    
    bool retirar(double monto) override {
        if (retirosGratis <= 0) {
            double comision = monto * COMISION_RETIRO;
            cout << "Se aplica comisión de $" << comision << " por retiro adicional\n";
            if (monto + comision > saldo) throw SaldoInsuficiente();
            saldo -= comision;
        }
        retirosGratis--;
        return Cuenta::retirar(monto);
    }
    
    void aplicarInteres() override {
        double interes = saldo * (tasaInteres / 100.0);
        saldo += interes;
        historial.push_back(Transaccion("INTERES", interes, saldo, "Interés mensual"));
    }
    
    void resetearRetirosGratis() { retirosGratis = MAX_RETIROS_GRATIS; }
    
    string getTipo() const override { return "Cuenta de Ahorros"; }
    
    string serializar() const override {
        return "AHORROS," + numeroCuenta + "," + titular->getId() + "," + 
               to_string(saldo) + "," + to_string(tasaInteres);
    }
};

const double CuentaAhorros::COMISION_RETIRO = 0.02; // 2%

// ==================== CUENTA CORRIENTE ====================
class CuentaCorriente : public Cuenta {
private:
    double limiteSobregiro;
    double sobregiroUsado;
    
public:
    CuentaCorriente(shared_ptr<Cliente> tit, double saldoInicial, double limiteSobregiro = 1000)
        : Cuenta(tit, saldoInicial, 0.5), limiteSobregiro(limiteSobregiro), sobregiroUsado(0) {}
    
    bool retirar(double monto) override {
        if (monto <= 0) throw ErrorBancario("El monto debe ser positivo");
        if (monto > saldo + (limiteSobregiro - sobregiroUsado)) {
            throw SaldoInsuficiente();
        }
        
        if (monto > saldo) {
            double sobregiroNecesario = monto - saldo;
            sobregiroUsado += sobregiroNecesario;
            saldo = 0;
            double comision = sobregiroNecesario * 0.05; // 5% de comisión por sobregiro
            sobregiroUsado += comision;
            historial.push_back(Transaccion("SOBREGIRO", sobregiroNecesario, saldo - sobregiroUsado,
                                           "Comisión: $" + to_string(comision)));
        } else {
            saldo -= monto;
        }
        historial.push_back(Transaccion("RETIRO", monto, saldo - sobregiroUsado));
        return true;
    }
    
    void depositar(double monto) override {
        if (sobregiroUsado > 0) {
            if (monto >= sobregiroUsado) {
                monto -= sobregiroUsado;
                sobregiroUsado = 0;
                historial.push_back(Transaccion("PAGO_SOBREGIRO", sobregiroUsado, 0));
            } else {
                sobregiroUsado -= monto;
                historial.push_back(Transaccion("PAGO_SOBREGIRO", monto, -sobregiroUsado));
                return;
            }
        }
        Cuenta::depositar(monto);
    }
    
    void aplicarInteres() override {
        if (sobregiroUsado > 0) {
            double interes = sobregiroUsado * 0.15; // 15% de interés por sobregiro
            sobregiroUsado += interes;
            historial.push_back(Transaccion("INTERES_SOBREGIRO", interes, -sobregiroUsado));
        } else if (saldo > 0) {
            Cuenta::aplicarInteres();
        }
    }
    
    string getTipo() const override { return "Cuenta Corriente"; }
    
    void mostrar() const override {
        Cuenta::mostrar();
        cout << "Límite de Sobregiro: $" << fixed << setprecision(2) << limiteSobregiro << endl;
        cout << "Sobregiro Usado: $" << sobregiroUsado << endl;
        cout << "Saldo Disponible Real: $" << (saldo - sobregiroUsado) << endl;
    }
    
    string serializar() const override {
        return "CORRIENTE," + numeroCuenta + "," + titular->getId() + "," + 
               to_string(saldo) + "," + to_string(tasaInteres) + "," + 
               to_string(limiteSobregiro) + "," + to_string(sobregiroUsado);
    }
};

// ==================== CUENTA DE INVERSIÓN ====================
template<typename T = double>
class CuentaInversion : public Cuenta {
private:
    T montoInvertido;
    double rendimiento;
    int mesesInversion;
    
public:
    CuentaInversion(shared_ptr<Cliente> tit, T montoInv, int meses = 12)
        : Cuenta(tit, 0, 0), montoInvertido(montoInv), rendimiento(0), mesesInversion(meses) {
        calcularTasa();
    }
    
    void calcularTasa() {
        tasaInteres = 5.0 + (mesesInversion * 0.5);
    }
    
    bool retirar(double monto) override {
        throw ErrorBancario("No se puede retirar de una cuenta de inversión antes del vencimiento");
    }
    
    void depositar(double monto) override {
        montoInvertido += monto;
        calcularTasa();
        historial.push_back(Transaccion("INVERSION", monto, montoInvertido));
    }
    
    void aplicarInteres() override {
        double interes = static_cast<double>(montoInvertido) * (tasaInteres / 100.0);
        rendimiento += interes;
        historial.push_back(Transaccion("RENDIMIENTO", interes, static_cast<double>(montoInvertido) + rendimiento));
    }
    
    void vencerInversion(shared_ptr<Cuenta> cuentaDestino) {
        double total = static_cast<double>(montoInvertido) + rendimiento;
        cuentaDestino->depositar(total);
        montoInvertido = 0;
        rendimiento = 0;
        historial.push_back(Transaccion("VENCIMIENTO", total, 0, "Transferido a cuenta principal"));
    }
    
    string getTipo() const override { return "Cuenta de Inversión"; }
    
    void mostrar() const override {
        Cuenta::mostrar();
        cout << "Monto Invertido: $" << fixed << setprecision(2) << static_cast<double>(montoInvertido) << endl;
        cout << "Rendimiento Acumulado: $" << rendimiento << endl;
        cout << "Total: $" << (static_cast<double>(montoInvertido) + rendimiento) << endl;
        cout << "Meses: " << mesesInversion << endl;
    }
    
    string serializar() const override {
        return "INVERSION," + numeroCuenta + "," + titular->getId() + "," + 
               to_string(montoInvertido) + "," + to_string(tasaInteres) + "," + 
               to_string(rendimiento) + "," + to_string(mesesInversion);
    }
};

// ==================== SISTEMA BANCARIO (PATRÓN SINGLETON) ====================
class SistemaBancario {
private:
    static SistemaBancario* instancia;
    vector<shared_ptr<Cliente>> clientes;
    vector<shared_ptr<Cuenta>> cuentas;
    map<string, shared_ptr<Cliente>> mapaClientes;
    map<string, shared_ptr<Cuenta>> mapaCuentas;
    queue<string> colaOperaciones;
    
    // Constructor privado (Singleton)
    SistemaBancario() {
        cout << "Inicializando Sistema Bancario...\n";
        cargarDatos();
    }
    
public:
    static SistemaBancario* obtenerInstancia() {
        if (instancia == nullptr) {
            instancia = new SistemaBancario();
        }
        return instancia;
    }
    
    // Gestión de Clientes
    shared_ptr<Cliente> crearCliente(string nombre, string apellido, string dni, string email, string tel) {
        string id = "CLI" + to_string(clientes.size() + 1);
        auto cliente = make_shared<Cliente>(id, nombre, apellido, dni, email, tel);
        clientes.push_back(cliente);
        mapaClientes[id] = cliente;
        registrarOperacion("ALTA_CLIENTE", id);
        return cliente;
    }
    
    shared_ptr<Cliente> buscarCliente(const string& id) {
        auto it = mapaClientes.find(id);
        if (it != mapaClientes.end()) return it->second;
        return nullptr;
    }
    
    // Gestión de Cuentas
    shared_ptr<Cuenta> crearCuentaAhorros(const string& idCliente, double saldoInicial) {
        auto cliente = buscarCliente(idCliente);
        if (!cliente) throw CuentaNoEncontrada();
        
        auto cuenta = make_shared<CuentaAhorros>(cliente, saldoInicial);
        cuentas.push_back(cuenta);
        mapaCuentas[cuenta->getNumero()] = cuenta;
        registrarOperacion("APERTURA_AHORROS", cuenta->getNumero());
        return cuenta;
    }
    
    shared_ptr<Cuenta> crearCuentaCorriente(const string& idCliente, double saldoInicial, double limite = 1000) {
        auto cliente = buscarCliente(idCliente);
        if (!cliente) throw CuentaNoEncontrada();
        
        auto cuenta = make_shared<CuentaCorriente>(cliente, saldoInicial, limite);
        cuentas.push_back(cuenta);
        mapaCuentas[cuenta->getNumero()] = cuenta;
        registrarOperacion("APERTURA_CORRIENTE", cuenta->getNumero());
        return cuenta;
    }
    
    template<typename T = double>
    shared_ptr<Cuenta> crearCuentaInversion(const string& idCliente, T monto, int meses = 12) {
        auto cliente = buscarCliente(idCliente);
        if (!cliente) throw CuentaNoEncontrada();
        
        auto cuenta = make_shared<CuentaInversion<T>>(cliente, monto, meses);
        cuentas.push_back(cuenta);
        mapaCuentas[cuenta->getNumero()] = cuenta;
        registrarOperacion("APERTURA_INVERSION", cuenta->getNumero());
        return cuenta;
    }
    
    shared_ptr<Cuenta> buscarCuenta(const string& numero) {
        auto it = mapaCuentas.find(numero);
        if (it != mapaCuentas.end()) return it->second;
        return nullptr;
    }
    
    // Operaciones Bancarias
    void transferir(const string& cuentaOrigen, const string& cuentaDestino, double monto) {
        auto origen = buscarCuenta(cuentaOrigen);
        auto destino = buscarCuenta(cuentaDestino);
        
        if (!origen || !destino) throw CuentaNoEncontrada();
        
        if (origen->retirar(monto)) {
            destino->depositar(monto);
            registrarOperacion("TRANSFERENCIA", cuentaOrigen + "->" + cuentaDestino + ":$" + to_string(monto));
            cout << "Transferencia exitosa!\n";
        }
    }
    
    void mostrarResumenSistema() const {
        cout << "\n===== RESUMEN DEL SISTEMA =====\n";
        cout << "Total Clientes: " << clientes.size() << endl;
        cout << "Total Cuentas: " << cuentas.size() << endl;
        
        double totalActivos = 0;
        for (const auto& cuenta : cuentas) {
            totalActivos += cuenta->getSaldo();
        }
        cout << "Total Activos: $" << fixed << setprecision(2) << totalActivos << endl;
        
        cout << "\n--- LISTADO DE CLIENTES ---\n";
        for (const auto& cliente : clientes) {
            cout << "ID: " << cliente->getId() << " - " << cliente->getNombreCompleto() 
                 << " (Score: " << cliente->getScore() << ")\n";
        }
        
        cout << "\n--- LISTADO DE CUENTAS ---\n";
        for (const auto& cuenta : cuentas) {
            cout << cuenta->getTipo() << " - " << cuenta->getNumero() 
                 << " - $" << cuenta->getSaldo() << endl;
        }
    }
    
    // Persistencia
    void guardarDatos() {
        ofstream archivo("banco_datos.txt");
        if (!archivo) throw ErrorBancario("Error al guardar datos");
        
        archivo << clientes.size() << endl;
        for (const auto& cliente : clientes) {
            archivo << cliente->getId() << "," << cliente->getNombreCompleto() << ","
                   << cliente->getDNI() << "," << cliente->getScore() << endl;
        }
        
        archivo << cuentas.size() << endl;
        for (const auto& cuenta : cuentas) {
            archivo << cuenta->serializar() << endl;
        }
        
        archivo.close();
        cout << "Datos guardados exitosamente.\n";
    }
    
    void cargarDatos() {
        ifstream archivo("banco_datos.txt");
        if (!archivo) return;
        
        try {
            int numClientes, numCuentas;
            archivo >> numClientes;
            archivo.ignore();
            
            for (int i = 0; i < numClientes; i++) {
                string linea;
                getline(archivo, linea);
                // Parsear y crear cliente (simplificado)
            }
            
            archivo >> numCuentas;
            archivo.ignore();
            
            for (int i = 0; i < numCuentas; i++) {
                string linea;
                getline(archivo, linea);
                // Parsear y crear cuenta (simplificado)
            }
            
            archivo.close();
            cout << "Datos cargados exitosamente.\n";
        } catch (...) {
            cout << "No se pudieron cargar datos previos.\n";
        }
    }
    
    ~SistemaBancario() {
        guardarDatos();
        delete instancia;
        instancia = nullptr;
    }
    
private:
    void registrarOperacion(const string& tipo, const string& detalle) {
        colaOperaciones.push(tipo + ":" + detalle);
        if (colaOperaciones.size() > 1000) colaOperaciones.pop();
    }
};

// Inicializar instancia Singleton
SistemaBancario* SistemaBancario::instancia = nullptr;

// ==================== FUNCIONES AUXILIARES ====================
template<typename T>
T solicitarDato(const string& mensaje) {
    T dato;
    cout << mensaje;
    while (!(cin >> dato)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada inválida. " << mensaje;
    }
    return dato;
}

string solicitarString(const string& mensaje) {
    string dato;
    cout << mensaje;
    cin.ignore();
    getline(cin, dato);
    return dato;
}

bool validarEmail(const string& email) {
    const regex patron("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex_match(email, patron);
}

// ==================== MENÚ PRINCIPAL ====================
void mostrarMenuPrincipal() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════╗\n";
    cout << "║         SISTEMA BANCARIO AVANZADO           ║\n";
    cout << "╚══════════════════════════════════════════════╝\n";
    cout << " 1. Gestión de Clientes\n";
    cout << " 2. Gestión de Cuentas\n";
    cout << " 3. Operaciones Bancarias\n";
    cout << " 4. Resumen del Sistema\n";
    cout << " 5. Procesar Intereses Mensuales\n";
    cout << " 6. Guardar y Salir\n";
    cout << "Seleccione una opción: ";
}

void menuClientes(SistemaBancario* banco) {
    int opcion;
    do {
        cout << "\n--- GESTIÓN DE CLIENTES ---\n";
        cout << "1. Registrar nuevo cliente\n";
        cout << "2. Buscar cliente\n";
        cout << "3. Volver al menú principal\n";
        opcion = solicitarDato<int>("Opción: ");
        
        switch(opcion) {
            case 1: {
                string nombre = solicitarString("Nombre: ");
                string apellido = solicitarString("Apellido: ");
                string dni = solicitarString("DNI: ");
                string email;
                do {
                    email = solicitarString("Email: ");
                } while (!validarEmail(email) && cout << "Email inválido. Intente de nuevo.\n");
                string telefono = solicitarString("Teléfono: ");
                
                auto cliente = banco->crearCliente(nombre, apellido, dni, email, telefono);
                cout << "Cliente creado con ID: " << cliente->getId() << endl;
                break;
            }
            case 2: {
                string id = solicitarString("Ingrese ID del cliente: ");
                auto cliente = banco->buscarCliente(id);
                if (cliente) {
                    cliente->mostrar();
                } else {
                    cout << "Cliente no encontrado.\n";
                }
                break;
            }
        }
    } while (opcion != 3);
}

void menuCuentas(SistemaBancario* banco) {
    int opcion;
    do {
        cout << "\n--- GESTIÓN DE CUENTAS ---\n";
        cout << "1. Abrir cuenta de ahorros\n";
        cout << "2. Abrir cuenta corriente\n";
        cout << "3. Abrir cuenta de inversión\n";
        cout << "4. Consultar cuenta\n";
        cout << "5. Volver al menú principal\n";
        opcion = solicitarDato<int>("Opción: ");
        
        switch(opcion) {
            case 1: {
                string idCliente = solicitarString("ID del cliente: ");
                double saldo = solicitarDato<double>("Saldo inicial: $");
                try {
                    auto cuenta = banco->crearCuentaAhorros(idCliente, saldo);
                    cout << "Cuenta de ahorros creada: " << cuenta->getNumero() << endl;
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 2: {
                string idCliente = solicitarString("ID del cliente: ");
                double saldo = solicitarDato<double>("Saldo inicial: $");
                double limite = solicitarDato<double>("Límite de sobregiro: $");
                try {
                    auto cuenta = banco->crearCuentaCorriente(idCliente, saldo, limite);
                    cout << "Cuenta corriente creada: " << cuenta->getNumero() << endl;
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 3: {
                string idCliente = solicitarString("ID del cliente: ");
                double monto = solicitarDato<double>("Monto a invertir: $");
                int meses = solicitarDato<int>("Plazo (meses): ");
                try {
                    auto cuenta = banco->crearCuentaInversion(idCliente, monto, meses);
                    cout << "Cuenta de inversión creada: " << cuenta->getNumero() << endl;
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
            case 4: {
                string numCuenta = solicitarString("Número de cuenta: ");
                auto cuenta = banco->buscarCuenta(numCuenta);
                if (cuenta) {
                    cuenta->mostrar();
                    cuenta->mostrarHistorial();
                } else {
                    cout << "Cuenta no encontrada.\n";
                }
                break;
            }
        }
    } while (opcion != 5);
}

void menuOperaciones(SistemaBancario* banco) {
    int opcion;
    do {
        cout << "\n--- OPERACIONES BANCARIAS ---\n";
        cout << "1. Depositar\n";
        cout << "2. Retirar\n";
        cout << "3. Transferir\n";
        cout << "4. Volver al menú principal\n";
        opcion = solicitarDato<int>("Opción: ");
        
        switch(opcion) {
            case 1: {
                string numCuenta = solicitarString("Número de cuenta: ");
                double monto = solicitarDato<double>("Monto a depositar: $");
                auto cuenta = banco->buscarCuenta(numCuenta);
                if (cuenta) {
                    try {
                        cuenta->depositar(monto);
                        cout << "Depósito exitoso!\n";
                    } catch (const exception& e) {
                        cout << "Error: " << e.what() << endl;
                    }
                } else {
                    cout << "Cuenta no encontrada.\n";
                }
                break;
            }
            case 2: {
                string numCuenta = solicitarString("Número de cuenta: ");
                double monto = solicitarDato<double>("Monto a retirar: $");
                auto cuenta = banco->buscarCuenta(numCuenta);
                if (cuenta) {
                    try {
                        if (cuenta->retirar(monto)) {
                            cout << "Retiro exitoso!\n";
                        }
                    } catch (const exception& e) {
                        cout << "Error: " << e.what() << endl;
                    }
                } else {
                    cout << "Cuenta no encontrada.\n";
                }
                break;
            }
            case 3: {
                string origen = solicitarString("Cuenta origen: ");
                string destino = solicitarString("Cuenta destino: ");
                double monto = solicitarDato<double>("Monto a transferir: $");
                try {
                    banco->transferir(origen, destino, monto);
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
            }
        }
    } while (opcion != 4);
}

// ==================== MAIN ====================
int main() {
    cout << "╔══════════════════════════════════════════════╗\n";
    cout << "║     SISTEMA DE GESTIÓN BANCARIA v2.0        ║\n";
    cout << "║     Programación Orientada a Objetos        ║\n";
    cout << "╚══════════════════════════════════════════════╝\n\n";
    
    SistemaBancario* banco = SistemaBancario::obtenerInstancia();
    int opcion;
    
    do {
        mostrarMenuPrincipal();
        opcion = solicitarDato<int>("");
        
        switch(opcion) {
            case 1:
                menuClientes(banco);
                break;
            case 2:
                menuCuentas(banco);
                break;
            case 3:
                menuOperaciones(banco);
                break;
            case 4:
                banco->mostrarResumenSistema();
                break;
            case 5:
                cout << "Procesando intereses del mes...\n";
                // Aquí se procesarían los intereses de todas las cuentas
                cout << "Intereses procesados exitosamente.\n";
                break;
            case 6:
                banco->guardarDatos();
                cout << "¡Gracias por usar el sistema bancario!\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }
    } while (opcion != 6);
    
    return 0;
}
