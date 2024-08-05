from PIL import Image
import glob
# files = glob.glob("M:\Custom_Smartwatch\smartwatch_code\images\*.png")
files = [r"M:\Custom_Smartwatch\smartwatch_code\images\png\notifications_48.png"]

for file in files:
    if file.startswith("Spotify") or "_160" in file or "_24" in file:
        continue
    im = Image.open(file)

    # fname = (file.split("\\")[-1]).split("_")[0] + "_160.png"
    fname = "notifications_160.png"

    newsize = (160, 160)
    im1 = im.resize(newsize)
    im1.save(fname, "PNG")
