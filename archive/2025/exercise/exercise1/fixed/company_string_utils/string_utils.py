# This component has ZERO external dependencies.
import base64

def capitalize(input_str: str) -> str:
    if not input_str:
        return input_str
    return input_str.capitalize()

def encode_base64(data: str) -> str:
    return base64.b64encode(data.encode('utf-8')).decode('utf-8')
