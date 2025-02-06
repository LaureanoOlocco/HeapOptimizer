---

## **1. Ventajas y desventajas de las políticas de asignación de memoria**

### **First Fit**
- **Ventajas**:  
  - Simple y rápido, ya que encuentra el primer bloque que cumple con el tamaño solicitado.  
  - Menor sobrecarga en comparación con otras políticas.  
- **Desventajas**:  
  - Alta fragmentación externa debido a la acumulación de pequeños bloques no utilizados.  
  - Puede dejar bloques grandes inaccesibles si se encuentran detrás de bloques más pequeños ocupados.  

### **Best Fit**
- **Ventajas**:  
  - Minimiza la fragmentación externa inmediata al usar el bloque más ajustado al tamaño solicitado.  
  - Mejora la reutilización de memoria.  
- **Desventajas**:  
  - Mayor tiempo de búsqueda, ya que revisa toda la lista de bloques libres.  
  - Puede generar fragmentación interna si los bloques asignados son ligeramente más grandes que lo necesario.  

### **Worst Fit**
- **Ventajas**:  
  - Minimiza la fragmentación interna al dejar bloques grandes libres para futuras asignaciones.  
  - Puede mejorar la asignación a largo plazo en sistemas con muchas solicitudes grandes.  
- **Desventajas**:  
  - Alta fragmentación externa si no se administra correctamente.  
  - Mayor sobrecarga al buscar el bloque más grande disponible.  

---

## **2. Compactación de bloques libres y reducción de fragmentación**

### **Compactación de bloques libres**
La compactación reduce la fragmentación externa al fusionar bloques libres adyacentes en uno más grande.  

### **Proceso implementado**
- En cada llamada a `free()`, se identifica si los bloques contiguos en el heap están libres.  
- Si se encuentran bloques adyacentes libres, la función `fusion()` combina estos bloques actualizando los metadatos.  
- Esto asegura que los bloques disponibles sean lo suficientemente grandes para futuras asignaciones, reduciendo la fragmentación externa.  

---

## **3. Funcionamiento y utilidad de mmap y munmap**

### **`mmap`**
- Mapea áreas de memoria en el espacio de direcciones de un proceso, permitiendo asignaciones grandes o específicas.  
- Admite memoria compartida y memoria mapeada desde archivos.  

### **`munmap`**
- Libera la memoria asignada con `mmap`.  

### **Cuándo usarlas en lugar de `sbrk`**
- Son más convenientes cuando se requiere granularidad y flexibilidad, como para asignar regiones específicas o manejar archivos grandes.  
- `sbrk` simplemente ajusta el final del heap y es más limitada en su capacidad de manejar memoria avanzada.  

---

## **4. Importancia de monitorear la tasa de fragmentación de memoria**

En sistemas con recursos limitados, la fragmentación de memoria puede llevar a:  
- **Falta de memoria utilizable**: A pesar de haber memoria libre, la fragmentación puede impedir asignaciones grandes.  
- **Degradación del rendimiento**: Incrementa los tiempos de búsqueda y puede generar más fallos en la administración de memoria.  

### **Beneficios de monitorear la fragmentación**
- Identificar patrones de uso ineficientes.  
- Optimizar políticas de asignación para mejorar el rendimiento y prolongar la vida útil del sistema.  

---

## **5. Herramientas de debugging y análisis dinámico aprendidas**

### **Valgrind**
- Detecta fugas de memoria, accesos inválidos y problemas de uso de memoria en tiempo de ejecución.  
- Crucial para verificar la correcta implementación de `malloc`, `free` y políticas de asignación.  

### **GDB**
- Herramienta poderosa para depurar paso a paso y analizar el comportamiento del programa.  
- Útil para identificar errores en funciones como `fusion()` o `check_heap()`, y entender cómo se administra el heap dinámicamente.  

---

## **Conclusión**
Las políticas de asignación tienen un impacto significativo en la eficiencia y fragmentación de memoria. La compactación y el uso de herramientas como `mmap` optimizan el sistema, mientras que Valgrind y GDB aseguran un desarrollo más robusto y confiable.
```