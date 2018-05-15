# Syd - Synchronize Directories

Desenvolvido por ITALO FRANCO, PIETRA FREITAS, VITOR VANACOR E VITÓRIA ROSA

## Compilar:
```
make
```
Os executáveis do cliente e servidor serão gerados na pasta `bin`

## Executar
### Servidor
```
./sydServer.exe <porta>
```
Se nenhuma porta for fornecida, será usada a default `4000`

### Cliente
```
./sydClient.exe <username> <hostname> <port>
```
Se algum parâmetro não for fornecido, serão utilizados os default `default_user localhost 4000`, respectivamente.

### Como usar

Ao se conectar com o servidor, será criada a pasta `sync_dir_<username>` na /home do usuário. Enquanto o cliente estiver rodando, todos os arquivos deixados nesta pasta serão sincronizados com a pasta `<username>` criada pelo servidor, e vice-versa.

## Comandos

- `upload <path/filename.ext>`: Envia arquivo para o servidor
- `download <filename.ext>`: Faz download do servidor para o diretório local
- `list_server` ou `ls`: Lista os arquivos salvos no servidor associados ao usuário.
- `list_client` ou `lc`: Lista os arquivos salvos no diretório `sync_dir_<nomeusuário>`
- `exit`: Encerra a sessão com o servidor
