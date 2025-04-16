from locust import HttpUser, task

class HelloWorldUser(HttpUser):
    @task
    def hello_world(self):
        self.client.get("/")
        self.client.get("/about.html")
        self.client.get("/contact.html")

#python3.11 -m locust -f client.py
