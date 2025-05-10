# GoMail - Exercise2
Bloom Filter TCP Server

in this file i will show you how to run this project that implement a server that support "GET", "POST", "DELETE" commands for managing a URL blacklist using a Bloom filter.

# clone the repository
```
 git clone https://github.com/basmalah-awawdeh/GoMail-Ex2
```

# running in Docker

This project supports full Docker-based execution using "docker-compose".

---

# 1st Terminal — Build and Run Containers

```bash
docker-compose up --build
```
This will start both the server and the client containers.

---

# 2nd Terminal — Start the Server Inside the Container

```bash
docker exec -it gomail-main-server-1 bash
./server 54321 8 1 2
```

# 3rd Terminal — Run the Client Inside the Container

```bash
docker exec -it gomail-main-client-1 bash
python3 client.py server 54321
```

You can now type commands like:

```
POST www.example.com
GET www.example.com
DELETE www.example.com
```

# running unit tests in Docker

```bash
docker exec -it gomail-main-server-1 bash
./tests
```

This will execute all unit tests inside the server container.


for example :

![Image](https://github.com/user-attachments/assets/5a3e2153-f375-4d44-8310-dea6b1e38d90)


# running the Application (Example)


*running the program 

![Image](https://github.com/user-attachments/assets/845967fc-7e6c-4bb8-9d28-8b11966ac45c)

![Image](https://github.com/user-attachments/assets/bb092a47-beb8-4b5e-9091-cd142d3fac36)

## 

*now we want to make sure that the data is not deleted after stopping the apllication

![Image](https://github.com/user-attachments/assets/2c125cce-d8a7-426c-971e-9de3283bfbfc)


