import re

counter = 0

def replacer(match):
    global counter
    counter += 1
    return f"Decor {counter}"

# Read the modified file (assumes each block starts with "Item")
with open("filtered_items.txt", "r") as infile:
    content = infile.read()

# Use a regex that looks for "Item" at the beginning of a line (ignoring any leading spaces)
new_content = re.sub(r'(?m)^\s*Item\b', replacer, content)

# Write the modified content to a new file
with open("decor_items.txt", "w") as outfile:
    outfile.write(new_content)

print(f"Replaced 'Item' with 'Decor <number>' in {counter} blocks.")