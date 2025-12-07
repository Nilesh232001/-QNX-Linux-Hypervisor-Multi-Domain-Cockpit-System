# Simple compositor prototype - reads two frame buffers (files) and composes them.
# This is a placeholder to show where a compositor would live.

import time
try:
    from PIL import Image
except Exception:
    print('Pillow not installed; compositor demo requires pillow.')
    raise

def compose(fb1_path, fb2_path, out_path):
    try:
        a = Image.open(fb1_path).convert('RGBA')
        b = Image.open(fb2_path).convert('RGBA')
        a.paste(b, (0,0), b)
        a.save(out_path)
        print('composed', out_path)
    except Exception as e:
        print('compose error', e)

if __name__ == '__main__':
    # Demo usage: create two small images and compose them
    from PIL import Image, ImageDraw
    a = Image.new('RGBA', (400,200), (30,30,30,255))
    b = Image.new('RGBA', (400,200), (0,0,0,0))
    d = ImageDraw.Draw(b)
    d.rectangle([50,50,350,150], outline=(255,0,0,255), width=5)
    b.save('/mnt/data/fb_cluster.png')
    a.save('/mnt/data/fb_ivi.png')
    compose('/mnt/data/fb_cluster.png','/mnt/data/fb_ivi.png','/mnt/data/fb_out.png')
