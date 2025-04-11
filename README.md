# Multi-User Communicating Shells with Shared Messaging (GTK + C Project)

> **C ile yazılmış, GTK GUI üzerinden çalışan çok kullanıcılı bir terminal uygulaması.**  
> Shell komutlarını çalıştırır, süreç yönetir, pipe destekler ve shared memory üzerinden mesajlaşma sağlar.

---

##  Projenin Temel Özellikleri

- ✅ Çoklu shell arayüzü (GUI tabanlı her shell ayrı pencere)
- ✅ Shell komutlarının çalıştırılması (`ls`, `cat`, `grep`, vs.)
- ✅ `|` (pipe) operatörü desteği
- ✅ Shared memory & semaphore ile mesajlaşma sistemi
- ✅ GTK+3 ile geliştirilmiş arayüz
- ✅ MVC (Model-View-Controller) mimarisi ile yapılandırma
- ✅ `fork`, `execvp`, `shm_open`, `mmap`, `sem_wait/post` gibi sistem çağrıları kullanılmıştır.

---

##  Teknolojiler ve Yöntemler

| Alan                | Kullanılan Yöntem/Gereç                           |
|---------------------|--------------------------------------------------|
| **Programlama Dili** | C (ANSI C standardına uygun)                     |
| **GUI**              | GTK+3 (GtkEntry, GtkTextView, GtkWindow)         |
| **Process Management** | `fork()`, `execvp()`, `wait()`                |
| **IPC (İşlemler Arası İletişim)** | POSIX `shm_open`, `mmap`, `sem_t`        |
| **Mimari**           | MVC (Model-View-Controller)                      |
| **Çıktı İşleme**     | `pipe()`, `dup2()`, `read()` ile stdout yönlendirme |
| **Senaryolar**       | Komut çalıştırma, mesaj gönderme, süreç gözlemi |

---

## Uygulama Ekran Görüntüsü

![project gui](A_description_of_a_project_is_displayed_in_a_scree.png)

---

## Kurulum ve Kullanım

```bash
sudo apt install libgtk-3-dev
git clone git@github.com:EmineCakal5/multiuser-shell-messaging.git
cd multiuser-shell-messaging
make
./proje
```

---

##  Komut Örnekleri

```bash
ls -l
ps aux | grep bash
@msg Merhaba diğer shell'lere!
echo Selam | wc -c
```

---

##  Geliştirici

**Emine Çakal**  
Bilgisayar Mühendisliği Öğrencisi  
- GitHub: [@EmineCakal5](https://github.com/EmineCakal5)  
- Proje: System Programming (C + GTK) — 2025 Bahar

---

##  Neden Bu Proje?

Bu proje sistem programlama, paralel süreç yönetimi, bellek paylaşımı ve GUI entegrasyonu gibi konuları bir araya getirerek C dilinde ileri seviye uygulama geliştirme pratiği sunmuştur. Aynı zamanda gerçek dünya projelerinde kullanılan `pipe`, `fork`, `exec`, `shm`, `semaphore`, `GUI loop` gibi kavramlar iç içe geçirilmiştir.

---

## Proje Yapısı

```
├── controller.c
├── model.c
├── view.c
├── Makefile
├── README.md
├── include/
│   └── model.h
```

---

## 🏁 Durum

Bu proje **bir grup çalışması olmaksızın tek başına geliştirilmiş**, tüm kod, yapılandırma ve entegrasyon adımları bizzat tarafımdan yürütülmüştür.  
Lab ortamında başarıyla sunulmuş ve test edilmiştir.
