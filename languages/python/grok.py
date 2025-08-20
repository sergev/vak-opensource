#!/usr/bin/env python3
#
# Send question to grok.com from command line.
# Save request as response.yaml, and reply as response.yaml.
# Get configuration from ~/.grok in INI format.
#
# Example of configuration:
#   [grok]
#   url = https://api.x.ai/v1/chat/completions
#   api_key = <your-api-key>
#   model = grok-3
#   system = You are a helpful assistant that can answer questions and help with tasks.
#
import json
import yaml
import os
import sys
import configparser
import requests

# Load configuration
config_path = os.path.expanduser("~/.grok")
if not os.path.exists(config_path):
    raise FileNotFoundError(f"Config file not found at {config_path}")
config_parser = configparser.ConfigParser()
if not config_parser.read(config_path):
    raise ValueError(f"Failed to read config file at {config_path}")
section_name = "grok"
if section_name not in config_parser:
    raise ValueError(f"Section [{section_name}] not found in {config_path}")
config  = config_parser[section_name]

# Get question from command-line arguments
if sys.argv[1:]:
    question = " ".join(sys.argv[1:]) + "?"
else:
    question = "?"

# Prepare request payload
payload = {
    "messages": [
        {
            "role": "system",
            "content": config['system']
        },
        {
            "role": "user",
            "content": question
        }
    ],
    "model": config['model']
}

# Write payload to temporary file
with open("request.yaml", "w") as rq_file:
    yaml.dump(payload, rq_file, default_flow_style=False, sort_keys = False)

# Make API request
headers = {
    "Content-Type": "application/json",
    "Authorization": f"Bearer {config['api_key']}"
}
response = requests.post(config['url'], headers=headers, data=json.dumps(payload))
response_data = response.json()

# Save response to file
with open("response.yaml", "w") as resp_file:
    yaml.dump(response_data, resp_file, default_flow_style=False, sort_keys = False)

# Parse and print response
if 'error' in response_data:
    print(response_data['code'])
    raise ValueError(response_data['error'])

print(response_data["choices"][0]["message"]["content"])
