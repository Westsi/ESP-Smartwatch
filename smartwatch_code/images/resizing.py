from PIL import Image
import glob
files = glob.glob("M:\Custom_Smartwatch\smartwatch_code\images\*.png")

for file in files:
    if file.startswith("Spotify"):
        continue
    im = Image.open(file)

    fname = (file.split("\\")[-1]).split("_")[0] + "_160"

    newsize = (160, 160)
    im1 = im.resize(newsize)
    im1.save(fname)
