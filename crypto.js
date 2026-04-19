 msg = f"GTR-CARD-{member_id}".encode()
    key = JWT_SECRET.encode()
    hex_digest = hmac.new(key, msg, hashlib.sha256).hexdigest()  # 64 hex chars

    digits = ""
    i = 0
    while len(digits) < 16 and i < 48:
        num = int(hex_digest[i : i + 3], 16) % 10
        digits += str(num)
        i += 3
    while len(digits) < 16:
        digits += "0"

    return f"{digits[0:4]} {digits[4:8]} {digits[8:12]} {digits[12:16]}"
