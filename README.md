# PD (Praca Dyplomowa)
Aplikacja nawiązująca i nadzorująca połączenie wideo z kamerą IP.
Mierzy paramerty połączenia takie jak ping, rozdzielczość transmitowanego sygnału oraz kodek wykorzystany do odtworzenia strumienia wideo.
Aplikacja wykrywa zamrożenie obrazu, czyli sytuację w której kamera odpowiada na ping, ale obraz nie zmienia się.

GUI utworzone z wykorzystaniem bibliotek QT.
Do dekodowania i analizy strumienia wideo wykorzystano biblioteki OpenCV oraz ffmpeg.
