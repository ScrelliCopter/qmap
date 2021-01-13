#!/usr/bin/env python3

import hashlib
import time
from urllib.parse import urljoin
from urllib.request import urlopen, Request as URLRequest
from urllib.error import URLError


def wget(uri: str, mirrors: list[str], sha256sum: str=None):
	for m in mirrors:
		url = urljoin(m, uri)
		headers = {
			"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:53.0) Gecko/20100101 Firefox/53.0"
		}

		try:
			print(f"Fetching \"{url}\"...")
			timeStart = time.time()
			response = urlopen(URLRequest(url, data=None, headers=headers))
			timeEnd = time.time()
			print(f"Responded in {(timeEnd - timeStart):.2f} sec")

			timeStart = time.time()
			payload = response.read()
			timeEnd = time.time()
			print(f"Downloaded {len(payload)} bytes in {(timeEnd - timeStart):.2f} sec")

			if sha256sum:
				digest = hashlib.sha256(payload).hexdigest()
				print(f"SHA256 {digest} - {'OK' if digest == sha256sum else 'mismatch'}")

		except URLError as e:
			print(e)


def main():
	file = "quake106.zip"
	uri = urljoin("idstuff/quake/", file)
	sha256sum = "ec6c9d34b1ae0252ac0066045b6611a7919c2a0d78a3a66d9387a8f597553239"
	idgamesMirrors = [
		# 😔 "ftp://ftp.idsoftware.com/",
		"https://youfailit.net/pub/idgames/",
		"https://www.quaddicted.com/files/idgames/",
		"https://dukeworld.com/idgames2/",
		"https://ftpmirror1.infania.net/pub/idgames/",
		"https://mirrors.syringanetworks.net/idgames/",
		"http://ftp.iinet.net/pub/",
		"http://www.gamers.org/pub/idgames/",
		"ftp://ftp.fu-berlin.de/pc/games/idgames/"
		# "ftp://ftp.fu-berlin.de/pc/msdos/games/idgames"
	]

	response = wget(uri, idgamesMirrors, sha256sum)


if __name__ == "__main__":
	main()
