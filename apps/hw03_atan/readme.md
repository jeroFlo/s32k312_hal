# Benchmark de funciones atan2

Tabla para registrar los resultados del benchmark en el S32K312.

## Pares de entrada

| Caso | `x` | `y` | Cuadrante | Valor esperado (rad) |
|---|---:|---:|---:|---:|
| Caso 1 | `0.80` | `0.20` | I | `0.244979` |
| Caso 2 | `-0.80` | `0.40` | II | `2.677945` |
| Caso 3 | `-0.90` | `-0.30` | III | `-2.819842` |
| Caso 4 | `0.70` | `-0.56` | IV | `-0.674741` |



| Test case | Caso 1: Output (rad) | Tiempo (us) | Tamano (bytes) | Caso 2: Output (rad) | Tiempo (us) | Caso 3: Output (rad) | Tiempo (us) | Caso 4: Output (rad) | Tiempo (us) |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Test 1: `atan2f` de `math.h` | `0.244978666` | `51.833` | `7952` | `2.67794514` | `56.5` | `-2.8198421` | `58.667` | `-0.67474097` | `66.25` |
| Test 2: `arm_atan2_f32` de CMSIS-DSP | `0.244978622` | `49.667` | `8160` | `2.67794514` | `56.917` | `-2.81984234` | `62.417` | `-0.67474097` | `55.583` |
| Test 3: `arm_atan2_q31` de CMSIS-DSP | `0.244978666` | `46.75` | `9152` | `2.67794514` | `48.417` | `-2.8198421` | `51.333` | `-0.67474097` | `50.75` |
| Test 4: Serie de Taylor en `float` | `0.244978681` | `39.75` | `7368` | `2.67790842` | `41.583` | `-2.81984162` | `46.33` | `-0.679823101` | `45.417` |
| Test 5: Serie de Taylor en Q31 | `0.244978681` | `27.833` | `7332` | `2.67790839` | `27.75` | `-2.81984162` | `29.917` | `-0.679823101` | `29.917` |
| Test 6: Tabla de consulta en `float` | `0.244978666` | `43.5` | `7508` | `2.67794514` | `45.667` | `-2.81985688` | `49.75` |`-0.674729347` | `47.75` |

## Estrategia de diseno del test 6

El test 6 aproxima `atan2(y, x)` usando una tabla de consulta en `float`. La
implementacion actual utiliza la siguiente estrategia:

1. Se almacena `atan(z)` para valores uniformemente espaciados de `z` en el intervalo `[0, 1]`.
2. Se calcula el cociente con signo `z_temp = y / x`.
3. Se separan la magnitud y el signo del cociente:

   ```text
   z = abs(z_temp)
   sign = sign(z_temp)
   ```

4. Para `z < 1`, se calcula la posicion dentro de la tabla y se localizan los dos valores vecinos.
5. Se aplica interpolacion lineal y se recupera el signo:

	```text
	theta = sign * (tabla[i] + fraccion * (tabla[i + 1] - tabla[i]))
	```

6. Finalmente, `atan_adjust_f32()` corrige el resultado segun los signos de `x` y `y` para obtener el cuadrante correcto de `atan2`.

La tabla utilizada tiene 65 valores, correspondientes a 64 intervalos entre `0` y `1`. Esto permite reducir el uso de memoria y realizar una interpolacion lineal con una resolucion de `1/64 = 0.015625` en el argumento.


## Medicion tamano de memoria 
Se tomo el text value\
jenifer@jenifer-Ubuntu:~/workspaceS32DS.3.6.7/s32k312_tareas/HW03_RAM$ arm-none-eabi-size --format=berkeley s32k312_tareas.elf
   text	   data	    bss	    dec	    hex	filename
  ` 8160`	   2480	    400	  11040	   2b20	s32k312_tareas.elf
