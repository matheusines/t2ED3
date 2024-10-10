# Diretiva para compilar o código
all:
	gcc -o programaTrab main.c cabecalho.c registro.c funcoes_fornecidas.c funcionalidades.c funcoesUteis.c funcoesArvoreB.c

# Diretiva para executar o código compilado
run: all
	./programaTrab

# Diretiva para limpar os arquivos compilados
clean:
	rm -f programaTrab
