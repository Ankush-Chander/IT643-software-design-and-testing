# This component only requires 'requests'.
import requests

def http_get(url: str) -> str:
    """Makes an HTTP GET request."""
    response = requests.get(url)
    response.raise_for_status()
    return response.text
