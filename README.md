# File transfer by socket TCP/UDP

###### tags: `computer communication networks` `lab1`

### 簡介
本次練習將利用`socket`傳送檔案，使用的`protocol`為`TCP`或`UDP`。
* 傳送的檔案大小須大於`200 MB`。
* 程式碼需附上註解。
* 記錄傳送資訊。
---
### 開發環境
* OS：Ubuntu 20.04.1 LTS (虛擬機)
* CPU：Intel® Core™ i5-1030NG7 CPU @ 1.10GHz × 4
* Memory：4 GiB
* Programming Language：C17
---
### 使用說明
* 編譯：
    ```bash=
    gcc -o socket_tcp_udp socket_tcp_udp.c
    ```
* 執行：
執行檔需要有五個參數：
依序為 `TCP/UDP`、`send/recv`、`ip`、`port`、`filename`。
    * `TCP`：
    ```bash=
    # for client
    ./socket_tcp_udp tcp send <ip> <port> <filename.txt>
    ```
    ```bash=
    # for server
    ./socket_tcp_udp tcp recv <ip> <port>
    ```
    * `UDP`：
    ```bash=
    # for client
    ./socket_tcp_udp udp send <ip> <port> <filename.txt>
    ```
    ```bash=
    # for server
    ./socket_tcp_udp udp recv <ip> <port>
    ```
---
### 執行結果
* `TCP`：
    ![](https://i.imgur.com/xoPDN7Q.png)
    * 左方為`server`端，右方為`client`端。
    * 紀錄訊息包含：
        * 每傳遞（接收）`25%`即顯示時間紀錄。
        * 總傳遞（接收）時間。
        * 檔案大小。
    * 接收端的檔名為：`receive_tcp.txt`。
    * **由於使用`TCP`作為傳輸的協定，因此傳送端與接收端的檔案大小一致（代表沒有任何封包丟失）。**
* `UDP`：
    ![](https://i.imgur.com/kuZkKOH.png)
    * 左方為`server`端，右方為`client`端。
    * 紀錄訊息包含：
        * 每傳遞（接收）`25%`即顯示時間紀錄。
        * 總傳遞（接收）時間。
        * 檔案大小。
        * 封包丟失率。
    * 接收端的檔名為：`receive_udp.txt`。
    * **因為封包有丟失，所以接收端最後一筆記錄並非`100%`，而是會`<100%`。**
    * **由於使用`UDP`作為傳輸的協定，因此在傳遞的過程會有封包丟失，所以接收端（`server`）收到的檔案大小少於傳送端（`client`）所送出的檔案。**


