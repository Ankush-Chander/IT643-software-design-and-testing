# In a shared library/component: 'company_common'
# This component's requirements.txt would need:
# requests
# psycopg2-binary

import base64
import requests  
import psycopg2  

# --- String Utilities ---
def capitalize(input_str: str) -> str:
    if not input_str:
        return input_str
    return input_str.capitalize()

def encode_base64(data: str) -> str:
    return base64.b64encode(data.encode('utf-8')).decode('utf-8')

# --- Network Utilities ---
def http_get(url: str) -> str:
    """Makes an HTTP GET request."""
    print(f"Making an HTTP GET request to {url}")
    response = requests.get(url)
    response.raise_for_status()
    return response.text

# --- Database Utilities ---
def get_db_connection(dsn: str):
    """Gets a PostgreSQL database connection."""
    print("Connecting to database...")
    # ... complex logic to get a DB connection ...
    # return psycopg2.connect(dsn) # dummy
    return None
