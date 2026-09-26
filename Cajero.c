#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PIN_CORRECTO 1234
#define INTENTOS_MAXIMOS 3
#define MAX_MOVIMIENTOS 5
#define SALDO_INICIAL 500000LL
#define MULTIPLO_RETIRO 10000LL

typedef struct {
	char tipo[12];
	long long monto;
} Movimiento;

static int leer_entero(const char *mensaje, long long *valor)
{
	char linea[128];
	char *fin;
	long long numero;

	printf("%s", mensaje);
	if (fgets(linea, sizeof(linea), stdin) == NULL) {
		return 0;
	}

	if (strchr(linea, '\n') == NULL && !feof(stdin)) {
		int caracter;
		while ((caracter = getchar()) != '\n' && caracter != EOF) {
		}
		return -1;
	}

	errno = 0;
	numero = strtoll(linea, &fin, 10);
	if (fin == linea || errno == ERANGE) {
		return -1;
	}
	while (isspace((unsigned char)*fin)) {
		fin++;
	}
	if (*fin != '\0') {
		return -1;
	}

	*valor = numero;
	return 1;
}

static void mostrar_dinero(long long monto)
{
	char digitos[32];
	size_t cantidad = 0;

	do {
		digitos[cantidad++] = (char)('0' + monto % 10);
		monto /= 10;
	} while (monto > 0 && cantidad < sizeof(digitos));

	printf("$");
	while (cantidad > 0) {
		putchar(digitos[--cantidad]);
		if (cantidad > 0 && cantidad % 3 == 0) {
			putchar('.');
		}
	}
}

static void registrar_movimiento(Movimiento movimientos[], int *cantidad,
								 const char *tipo, long long monto)
{
	int indice;

	if (*cantidad == MAX_MOVIMIENTOS) {
		for (indice = 1; indice < MAX_MOVIMIENTOS; indice++) {
			movimientos[indice - 1] = movimientos[indice];
		}
		*cantidad = MAX_MOVIMIENTOS - 1;
	}

	snprintf(movimientos[*cantidad].tipo, sizeof(movimientos[*cantidad].tipo),
			 "%s", tipo);
	movimientos[*cantidad].monto = monto;
	(*cantidad)++;
}

static int validar_pin(void)
{
	int intento;
	long long pin;

	for (intento = 1; intento <= INTENTOS_MAXIMOS; intento++) {
		int resultado = leer_entero("Ingrese su PIN: ", &pin);

		if (resultado == 0) {
			return 0;
		}
		if (resultado == 1 && pin == PIN_CORRECTO) {
			printf("Acceso concedido.\n");
			return 1;
		}

		printf("PIN incorrecto. Intentos restantes: %d\n",
			   INTENTOS_MAXIMOS - intento);
	}

	printf("Ha agotado los intentos. El cajero se cerrara.\n");
	return 0;
}

static int consultar_monto(long long *monto)
{
	int resultado;

	while ((resultado = leer_entero("Ingrese el monto en pesos: ", monto)) < 0) {
		printf("Entrada invalida. Digite un monto usando solo numeros.\n");
	}
	return resultado;
}

int main(void)
{
	long long saldo = SALDO_INICIAL;
	long long opcion;
	Movimiento movimientos[MAX_MOVIMIENTOS];
	int cantidad_movimientos = 0;
	int continuar = 1;
	int resultado_opcion;

	printf("=== Cajero automatico ===\n");
	if (!validar_pin()) {
		return 0;
	}

	do {
		printf("\n1. Consultar saldo\n");
		printf("2. Depositar dinero\n");
		printf("3. Retirar dinero\n");
		printf("4. Ver ultimos movimientos\n");
		printf("5. Salir\n");

		resultado_opcion = leer_entero("Seleccione una opcion: ", &opcion);
		if (resultado_opcion == 0) {
			break;
		}
		if (resultado_opcion < 0) {
			printf("Opcion invalida. Seleccione un numero del 1 al 5.\n");
			continue;
		}

		switch (opcion) {
		case 1:
			printf("Saldo disponible: ");
			mostrar_dinero(saldo);
			printf("\n");
			break;

		case 2: {
			long long monto;
			int resultado;

			do {
				resultado = consultar_monto(&monto);
				if (resultado == 0) {
					continuar = 0;
					break;
				}
				if (monto <= 0) {
					printf("El deposito debe ser mayor que cero.\n");
				}
			} while (monto <= 0);

			if (resultado == 0) {
				break;
			}
			if (monto > LLONG_MAX - saldo) {
				printf("El monto supera el limite permitido.\n");
			} else {
				saldo += monto;
				registrar_movimiento(movimientos, &cantidad_movimientos,
								 "Deposito", monto);
				printf("Deposito realizado. Nuevo saldo: ");
				mostrar_dinero(saldo);
				printf("\n");
			}
			break;
		}

		case 3: {
			long long monto;
			int resultado = consultar_monto(&monto);

			if (resultado == 0) {
				continuar = 0;
			} else if (monto <= 0) {
				printf("El retiro debe ser mayor que cero.\n");
			} else if (monto > saldo) {
				printf("Fondos insuficientes. Su saldo es: ");
				mostrar_dinero(saldo);
				printf("\n");
			} else if (monto % MULTIPLO_RETIRO != 0) {
				printf("El retiro debe ser multiplo de $10.000.\n");
			} else {
				saldo -= monto;
				registrar_movimiento(movimientos, &cantidad_movimientos,
									 "Retiro", monto);
				printf("Retiro realizado. Nuevo saldo: ");
				mostrar_dinero(saldo);
				printf("\n");
			}
			break;
		}

		case 4:
			if (cantidad_movimientos == 0) {
				printf("No hay movimientos registrados.\n");
			} else {
				printf("Ultimos movimientos (del mas antiguo al mas reciente):\n");
				for (int indice = 0; indice < cantidad_movimientos; indice++) {
					printf("%d. %s: ", indice + 1, movimientos[indice].tipo);
					mostrar_dinero(movimientos[indice].monto);
					printf("\n");
				}
			}
			break;

		case 5:
			continuar = 0;
			printf("Gracias por usar el cajero.\n");
			break;

		default:
			printf("Opcion invalida. Seleccione un numero del 1 al 5.\n");
			break;
		}
	} while (continuar);

	return 0;
}
