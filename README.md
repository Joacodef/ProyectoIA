# ProyectoIA

Intento de resolver las instancias "AB" del GVRP utilizando backtracking, sin usar prácticamente ninguna librería de C++. Se utiliza una lista enlazada básica como estructura de datos principal (creada desde cero). Constituye el primer intento de realizar este proyecto para el ramo de IA de la usm.

Resultados: La lista enlazada tiene bastantes leaks de memoria, y hubo errores conceptuales en cómo se debía implementar el backtracking. Por lo tanto, se decidió rehacer casi todo el tema de la lista enlazada, utilizando la librería de "vector", para no tener directamente con malloc y free. También se cambió la idea general sobre cómo se aplicaría backtracking.

# Instrucciones

Utilizar comando "make" para compilar (con el comando "g++") y ejecutar.

Utilizar "make clean" para limpiar el archivo de output.

Se necesita que estén todos los archivos ".h" y el ".cpp" en la misma carpeta.

Se necesita que estén las carpetas "Instancias" con las intancias AB101-AB120 y AB201-AB220.

Se necesita que esté la carpeta "Soluciones" (vacía o no vacía) para generar las soluciones.
