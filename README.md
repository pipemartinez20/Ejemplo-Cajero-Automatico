# Cajero automático

## Descripción

Este programa simula un cajero automático en la terminal. Permite consultar el saldo, depositar y retirar dinero, y revisar los últimos movimientos. El saldo inicial es de **$500.000**.

Para ingresar, utiliza el PIN **1234**. Hay un máximo de **3 intentos**.

## Cómo compilar

Desde la carpeta del proyecto, ejecuta:

```bash
gcc -std=c11 -Wall -Wextra -pedantic Cajero.c -o cajero
```

## Cómo ejecutar

Después de compilar, ejecuta:

```bash
./cajero
```

## Opciones del menú

1. **Consultar saldo:** muestra el saldo disponible.
2. **Depositar dinero:** agrega un monto mayor que cero al saldo.
3. **Retirar dinero:** retira un monto mayor que cero, siempre que haya fondos suficientes. El monto debe ser múltiplo de **$10.000**.
4. **Ver últimos movimientos:** muestra hasta los 5 movimientos más recientes.
5. **Salir:** cierra el programa.
