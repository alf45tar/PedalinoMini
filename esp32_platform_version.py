import subprocess

def get_platform_version():
    result = subprocess.run(['pio', 'pkg', 'show', 'espressif32'], stdout=subprocess.PIPE)
    output = result.stdout.decode()
    for line in output.splitlines():
        if "Platform" in line:
            return line.split()[2]

Import("env")

platform_version = get_platform_version()
print(f"ESP32 Platform Version: {platform_version}")

# Pass the version to the compiler
env.Append(CPPDEFINES=[("ESP32_PLATFORM_VERSION", f'"{platform_version}"')])