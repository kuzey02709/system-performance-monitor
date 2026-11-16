EN

System Performance Monitor - Lightweight Hardware Telemetry & REST API
A lightweight system performance monitoring tool combining low-level C system metric extraction with a Python (Flask) backend API and real-time web dashboard.

Technologies Used
C (GCC / Win32 API or /proc filesystem) - Low-level OS and hardware metric extraction with minimal resource footprint

Python 3 & Flask - RESTful backend API bridging the C executable and formatting raw metrics into structured JSON responses

HTML5 & CSS3 - Clean, responsive dashboard interface for real-time telemetry visualization

JavaScript (Fetch API) - Asynchronous data polling and dynamic UI updates

System Architecture and Flow
Data Extraction Layer: The compiled C module directly queries OS and hardware-level metrics including CPU usage, memory allocation, and system load.

API & Serving Layer: The Flask backend executes or communicates with the C layer, serializing raw hardware telemetry into structured JSON endpoints.

Real-Time Dashboard: The frontend periodically fetches JSON payloads from the API to update system performance metrics live without page reloads.

Installation & Execution
Compile the C module using gcc main.c -o main, install Flask via pip install flask, and start the server with python app.py.

TR

System Performance Monitor - Hafif Donanım Telemetrisi ve REST API
C diline dayalı düşük seviyeli sistem metrik çıkarımını, Python (Flask) arka uç API'si ve gerçek zamanlı web kontrol paneliyle birleştiren hafif bir sistem performans izleme aracı.

Kullanılan Teknolojiler
C (GCC / Win32 API veya /proc dosya sistemi) - Minimum kaynak tüketimiyle düşük seviyeli işletim sistemi ve donanım metrik çıkarımı

Python 3 ve Flask - C modülü ile iletişim kuran ve ham metrikleri yapılandırılmış JSON yanıtlarına dönüştüren RESTful arka uç API'si

HTML5 ve CSS3 - Gerçek zamanlı telemetri görselleştirmesi için sade ve duyarlı kontrol paneli arayüzü

JavaScript (Fetch API) - Asenkron veri sorgulama ve dinamik arayüz güncellemeleri

Sistem Mimarisi ve Akışı
Veri Çıkarım Katmanı: Derlenmiş C modülü, CPU kullanımı, bellek tahsisi ve sistem yükü dahil olmak üzere işletim sistemi ve donanım seviyesindeki metrikleri doğrudan sorgular.

API ve Sunum Katmanı: Flask arka ucu C katmanıyla haberleşerek ham donanım telemetrisini yapılandırılmış JSON uç noktalarına dönüştürür.

Gerçek Zamanlı Kontrol Paneli: Ön yüz, sayfa yenilemeye gerek kalmadan sistem performans metriklerini canlı olarak güncellemek için API'den periyodik olarak JSON verilerini çeker.

Kurulum ve Çalıştırma
gcc main.c -o main komutuyla C modülünü derleyin, pip install flask ile bağımlılığı yükleyin ve python app.py komutuyla sunucuyu başlatın.
