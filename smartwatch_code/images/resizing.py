from PIL import Image
import glob
files = glob.glob("/Users/jamie/Documents/Code/smartwatch/smartwatch_code/images/myicons/*.PNG")
# files = [r"/Users/jamie/Documents/Code/smartwatch/smartwatch_code/images/myicons/"]
print(files)

for file in files:
    if file.startswith("Spotify") or "_160" in file or "_24" in file:
        continue
    im = Image.open(file)

    fname = "/Users/jamie/Documents/Code/smartwatch/smartwatch_code/images/myicons/" + (file.split("/")[-1]).split(".")[0] + "_160.png"
    print(fname)
    # fname = "cool_emoji_160.png"

    newsize = (160, 160)
    im1 = im.resize(newsize)
    im1.save(fname, "PNG")
