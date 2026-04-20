const readline = require("readline");

const rl = readline.createInterface({
input: process.stdin,
output: process.stdout
});

let tasks = [];

function showMenu() {
console.log("\n--- Gestor de Tareas ---");
console.log("1. Agregar tarea");
console.log("2. Ver tareas");
console.log("3. Eliminar tarea");
console.log("4. Salir");
rl.question("Selecciona una opción: ", handleMenu);
}

function handleMenu(option) {
switch(option) {
case "1":
addTask();
break;
case "2":
viewTasks();
break;
case "3":
deleteTask();
break;
case "4":
exitApp();
break;
default:
console.log("Opción inválida");
showMenu();
}
}

function addTask() {
rl.question("Escribe la tarea: ", (task) => {
tasks.push(task);
console.log("Tarea agregada");
showMenu();
});
}

function viewTasks() {
if (tasks.length === 0) {
console.log("No hay tareas");
} else {
tasks.forEach((task, index) => {
console.log(`${index + 1}. ${task}`);
});
}
showMenu();
}

function deleteTask() {
viewTasks();
rl.question("Número de tarea a eliminar: ", (num) => {
tasks.splice(num - 1, 1);
console.log("Tarea eliminada");
showMenu();
});
}

function exitApp() {
console.log("Adiós");
rl.close();
}

showMenu();
