run_server:
	python Server/main.py

run_client:
	javac Client/*.java
	java Client.GUI

zip:
	zip -R poo_final.zip . '*'