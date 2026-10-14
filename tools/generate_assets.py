import random, math
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw

ROOT_DIR = Path(__file__).resolve().parents[1]
ASSETS_DIR = ROOT_DIR / 'assets'
SPRITES_DIR = ASSETS_DIR / 'sprites'
PROPS_DIR = SPRITES_DIR / 'props'
TILESETS_DIR = ASSETS_DIR / 'tilesets'
PREVIEWS_DIR = ASSETS_DIR / 'previews'
for output_dir in (SPRITES_DIR, PROPS_DIR, TILESETS_DIR, PREVIEWS_DIR):
    output_dir.mkdir(parents=True, exist_ok=True)

def H(h): h=h.lstrip('#'); return tuple(int(h[i:i+2],16) for i in (0,2,4))+(255,)
P = dict(
 ink='0b1a22', deep='14303a', teal='1d4a52', teal2='2a5f62',
 jd1='1f5a48', jd2='2f7a5a', jd3='46995f', jd4='72bb77', jd5='a8d68a',
 amber='e8b867', pale='f4d58d', glow='ffe9a8',
 st1='3b4c4c', st2='55685f', st3='7a8d80', st4='a3b19b', st5='c5cfb8',
 sand1='b79f6f', sand2='d4bf8a', sand3='ebdcab',
 dirt1='4a3626', dirt2='6a4d35', dirt3='8a6a49',
 w1='1b5570', w2='2a7d93', w3='5db8bd', w4='bfe8df',
 red1='8f323f', red2='c9505a', skin='e1b58e', skin2='b98a69', coat='2a5560', coat2='1d3d46',
 hair='171d27', belt='e0a85a', pant='26363f', boot='141c22')
P = {k:H(v) for k,v in P.items()}
T=16
rnd=random.Random(7)

def new(w=T,h=T,c=(0,0,0,0)): return Image.new('RGBA',(w,h),c)

def speckle(img, base, cols, density, seed):
    r=random.Random(seed); px=img.load()
    for y in range(img.height):
        for x in range(img.width):
            px[x,y]=base
            if r.random()<density: px[x,y]=r.choice(cols)

def grass(seed=1):
    im=new(); speckle(im,P['jd2'],[P['jd1'],P['jd3'],P['jd1']],.10,seed)
    r=random.Random(seed*3); px=im.load()
    for _ in range(4):
        x,y=r.randrange(1,15),r.randrange(2,15)
        px[x,y]=P['jd4']; px[x,y-1]=P['jd5'] if r.random()<.4 else P['jd4']
    return im
def jungle(seed=2):
    im=new(); speckle(im,P['jd1'],[P['deep'],P['jd2'],P['deep']],.14,seed)
    r=random.Random(seed*5); px=im.load()
    for _ in range(3):
        x,y=r.randrange(1,14),r.randrange(1,15); px[x,y]=P['jd3']; px[x+1,y]=P['jd2']
    return im
def dirt(seed=3):
    im=new(); speckle(im,P['dirt2'],[P['dirt1'],P['dirt3'],P['dirt1']],.13,seed)
    r=random.Random(seed*7); px=im.load()
    for _ in range(3): px[r.randrange(16),r.randrange(16)]=P['sand1']
    return im
def sand(seed=4):
    im=new(); speckle(im,P['sand2'],[P['sand3'],P['sand1'],P['sand3']],.2,seed); return im
def stone(seed=5):
    im=new(); speckle(im,P['st3'],[P['st2'],P['st4']],.15,seed); d=ImageDraw.Draw(im)
    for (x0,y0,x1,y1) in [(0,0,15,0),(0,8,15,8),(0,0,0,7),(8,8,8,15),(8,0,8,0)]:
        d.line((x0,y0,x1,y1),fill=P['st2'])
    d.line((1,1,14,1),fill=P['st4']); d.line((1,9,14,9),fill=P['st4'])
    return im
def cliff(seed=6):
    im=new(); d=ImageDraw.Draw(im)
    speckle(im,P['st2'],[P['st1'],P['st3'],P['st1']],.22,seed)
    d.rectangle((0,0,15,2),fill=P['st4']); d.line((0,3,15,3),fill=P['st5'])
    d.line((0,12,15,12),fill=P['st1']); d.rectangle((0,13,15,15),fill=P['teal'])
    for x in range(2,16,5): d.line((x,5,x,10),fill=P['st1'])
    return im
def ramp():
    im=new(); d=ImageDraw.Draw(im)
    for i in range(8):
        y=i*2; c=P['st4'] if i%2==0 else P['st3']
        d.rectangle((0,y,15,y+1),fill=c)
        d.line((0,y+1,15,y+1),fill=P['st2'])
    d.rectangle((0,0,1,15),fill=P['st1']); d.rectangle((14,0,15,15),fill=P['st1'])
    return im
def water(f):
    im=new(); d=ImageDraw.Draw(im); d.rectangle((0,0,15,15),fill=P['w1'])
    r=random.Random(11); px=im.load()
    for y in range(16):
        for x in range(16):
            if r.random()<.2: px[x,y]=P['w2']
    for (x,y) in [(2,3),(9,7),(4,12),(12,1)]:
        xx=(x+f*2)%14
        d.line((xx,y,xx+3,y),fill=P['w3'])
    d.point(((6+f*3)%15,9),fill=P['w4'])
    return im
def thorn():
    im=new(); d=ImageDraw.Draw(im)
    for i,x in enumerate(range(1,16,4)):
        d.line((x,15,x+(i%2)*2-1,3),fill=P['jd1'])
        d.line((x+1,15,x+(i%2)*2,4),fill=P['jd2'])
        d.point((x-1,8),fill=P['red2']); d.point((x+2,6),fill=P['red2']); d.point((x,11),fill=P['red1'])
    d.line((0,10,15,6),fill=P['dirt2'])
    return im

# --- sprites ---
SPR={
'down':["....KKKKKK....","...KKKKKKKK...","..KKKKKKKKKK..","..KKSSSSSSKK..","..KSSSSSSSSK..","..KSESSSSESK..","...SSSSSSSS...","....RRRRRR....","...RRRRrRRR...","..STTTTRTTTS..",".STTTTTRTTTTS.",".STTTTTRTTTTS.","..TTTTTRTTTT..","..GGGGGGGGGG..","..TTTTTTTTTT..","...PPPPPPPP..."],
'up':  ["....KKKKKK....","...KKKKKKKK...","..KKKKKKKKKK..","..KKKKKKKKKK..","..KKKKKKKKKK..","..KKKKKKKKKK..","...KKKKKKKK...","....RRRRRR....","...RRRRRRRR...","..STTTTrrTTS..",".STTTTTrrTTTS.",".STTTTTrrTTTS.","..TTTTTrrTTT..","..GGGGGGGGGG..","..TTTTTTTTTT..","...PPPPPPPP..."],
'right':["....KKKKKK....","...KKKKKKKK...","..KKKKKKKKKK..","..KKKKSSSSK...","..KKKSSSSSK...","..KKKSSSESK...","...KKSSSSS....","..rRRRRRRR....",".rrRRRRRR.....","..rTTTTTTT....","...TTTTTTTS...","...TTTTTTTS...","...TTTTTTT....","...GGGGGGG....","...TTTTTTT....","....PPPPPP...."],
}
LEGS={
 'idle':["...PPP..PPP...","...PPP..PPP...","...BBB..BBB...","..BBBB..BBBB.."],
 'wA':  ["...PPP..PPP...","...PPP...PP...","...BBB...BBB..","..BBBB...BBBB."],
 'wB':  ["...PPP..PPP...","...PP...PPP...","..BBB...BBB...",".BBBB...BBBB.."],
}
LEGS_SIDE={
 'idle':["....PPP.PPP...","....PPP.PPP...","....BBB.BBB...","...BBBB.BBBB.."],
 'wA':  ["....PPPPPP....",".....PPPPP....","....BBB..BBB..","...BBBB..BBBB."],
 'wB':  ["....PPPPPP....","....PPPPP.....","...BBB..BBB...","..BBBB.BBBB..."],
}
KEY=dict(K='hair',S='skin',E='ink',T='coat',t='coat2',R='red2',r='red1',G='belt',P='pant',B='boot')
def sprite(dirn,pose):
    rows=SPR['right' if dirn=='left' else dirn]+(LEGS_SIDE if dirn in('left','right') else LEGS)[pose]
    im=new(14,20); px=im.load()
    for y,row in enumerate(rows):
        for x,ch in enumerate(row):
            if ch in KEY: px[x,y]=P[KEY[ch]]
    if dirn=='left': im=im.transpose(Image.FLIP_LEFT_RIGHT)
    return im

def palm(seed=1):
    r=random.Random(seed); im=new(40,56); d=ImageDraw.Draw(im)
    # shadow
    d.ellipse((8,50,32,55),fill=(8,24,28,110))
    # trunk (gentle curve)
    pts=[]
    for i in range(0,34):
        x=20+int(3*math.sin(i/34*math.pi*0.9)); y=52-i
        d.rectangle((x-2,y,x+1,y),fill=P['dirt2']); d.point((x-2,y),fill=P['dirt1']); d.point((x+1,y),fill=P['dirt3'])
        if i%4==0: d.line((x-2,y,x+1,y),fill=P['dirt1'])
        pts.append((x,y))
    cx,cy=pts[-1]
    fronds=[(-17,-2),(-13,-9),(-6,-13),(6,-13),(13,-9),(17,-2),(-9,5),(9,5),(0,-14)]
    for (dx,dy) in fronds:
        ex,ey=cx+dx,cy+dy; mx,my=cx+dx*0.5,cy+dy*0.5-5
        for t in np.linspace(0,1,24):
            x=(1-t)**2*cx+2*(1-t)*t*mx+t*t*ex; y=(1-t)**2*cy+2*(1-t)*t*my+t*t*(ey+7)
            w=int(4*(1-t))+1
            d.rectangle((x-w,y-1,x+w,y+1),fill=P['jd3'])
            d.line((x-w,y-1,x+w,y-1),fill=P['jd5'] if t<.7 else P['jd4']); d.point((x-w,y+1),fill=P['jd1']); d.point((x+w,y+1),fill=P['jd1'])
    d.ellipse((cx-2,cy-2,cx+2,cy+2),fill=P['dirt3'])
    return im
def bush():
    im=new(20,16); d=ImageDraw.Draw(im); d.ellipse((1,13,18,15),fill=(8,24,28,100))
    for (x,y,r,c) in [(6,9,5,'jd1'),(13,9,5,'jd1'),(10,6,6,'jd2'),(7,8,3,'jd3'),(12,5,2,'jd4')]:
        d.ellipse((x-r,y-r,x+r,y+r),fill=P[c])
    for (x,y) in [(6,8),(13,7)]: d.point((x,y),fill=P['red2'])
    return im
def marker():
    im=new(16,32); d=ImageDraw.Draw(im); d.ellipse((1,28,14,31),fill=(8,24,28,110))
    d.rectangle((3,10,12,29),fill=P['st3']); d.rectangle((3,10,5,29),fill=P['st4']); d.rectangle((10,10,12,29),fill=P['st2'])
    d.polygon([(2,10),(8,2),(13,10)],fill=P['st4']); d.line((3,9,8,3),fill=P['st5'])
    d.rectangle((6,16,9,17),fill=P['glow']); d.rectangle((7,14,8,19),fill=P['glow'])
    for y in (22,25): d.line((5,y,10,y),fill=P['teal2'])
    return im
def pillar():
    im=new(16,40); d=ImageDraw.Draw(im); d.ellipse((0,35,15,39),fill=(8,24,28,110))
    d.rectangle((3,4,12,35),fill=P['st3']); d.rectangle((3,4,5,35),fill=P['st4']); d.rectangle((11,4,12,35),fill=P['st2'])
    d.rectangle((1,0,14,4),fill=P['st4']); d.rectangle((1,34,14,37),fill=P['st2'])
    for y in range(8,32,5): d.line((6,y,9,y),fill=P['st2'])
    d.point((8,12),fill=P['jd3']); d.point((7,30),fill=P['jd3'])
    return im

tiles=dict(G=grass(1),G2=grass(9),J=jungle(2),J2=jungle(8),D=dirt(3),D2=dirt(12),A=sand(4),A2=sand(13),S=stone(5),N=cliff(6),R=ramp(),W0=water(0),W1=water(1),T=thorn())

# tileset sheet
sheet=new(T*8,T*2)
for i,k in enumerate(['G','G2','J','J2','D','D2','A','A2','S','N','R','W0','W1','T']):
    sheet.paste(tiles[k],((i%8)*T,(i//8)*T))
tileset_path = TILESETS_DIR / 'tileset_16.png'
sheet.save(tileset_path)

# player sheet: rows dirs (down,left,right,up), cols idle,wA,idle,wB
ps=new(14*4,20*4)
for r,dn in enumerate(['down','left','right','up']):
    for c,pose in enumerate(['idle','wA','idle','wB']):
        ps.paste(sprite(dn,pose),(c*14,r*20))
player_path = SPRITES_DIR / 'player_sheet_14x20.png'
ps.save(player_path)
for n,i in [('palm',palm(1)),('bush',bush()),('marker',marker()),('pillar',pillar())]:
    i.save(PROPS_DIR / f'prop_{n}.png')

# ---- preview scene ----
LAY=["JJJJJJJJJJJJJJJJJJJJ",
     "JJGGJJJSSSSSSJJGGGJJ",
     "JGGGGJJSSSSSSJJGGGGJ",
     "JGGGGJJNNRRNNJGGGGGJ",
     "GGGGGGGGGDDGGGGGGGGG",
     "GGGGGGGGGDDGGGGGGGGG",
     "GGGGGGGDDDDDDGGGGGGG",
     "GGGGGGGDDGGGDDGGGGAA",
     "GGGGGGGDDGGGGDDGGAAW",
     "GGGGGGGGDDGGGGDDAAWW",
     "GGGGGGGGGDDGGGGAAWWW",
     "GGGGGGGGGGDDGGAAWWWW"]
W,Hh=20*T,12*T
scene=new(W,Hh)
r=random.Random(5)
for y,row in enumerate(LAY):
    for x,ch in enumerate(row):
        if ch=='G': t=tiles['G'] if r.random()<.5 else tiles['G2']
        elif ch=='J': t=tiles['J'] if r.random()<.5 else tiles['J2']
        elif ch=='D': t=tiles['D'] if r.random()<.5 else tiles['D2']
        elif ch=='A': t=tiles['A'] if r.random()<.5 else tiles['A2']
        elif ch=='W': t=tiles['W0']
        else: t=tiles[ch]
        scene.paste(t,(x*T,y*T))
# soft shore foam + sand-to-water edge, dirt edge shading
d=ImageDraw.Draw(scene)
for y,row in enumerate(LAY):
    for x,ch in enumerate(row):
        if ch=='W' and x>0 and LAY[y][x-1]!='W': d.line((x*T,y*T,x*T,y*T+15),fill=P['w4'])
        if ch=='S' and y>0 and LAY[y-1][x]!='S': pass
for y,row in enumerate(LAY):
    for x,ch in enumerate(row):
        if ch=='D':
            for dx,dy,(x0,y0,x1,y1) in [(-1,0,(0,0,0,15)),(1,0,(15,0,15,15)),(0,-1,(0,0,15,0)),(0,1,(0,15,15,15))]:
                nx,ny=x+dx,y+dy
                if 0<=nx<20 and 0<=ny<12 and LAY[ny][nx] in 'GJ':
                    d.line((x*T+x0,y*T+y0,x*T+x1,y*T+y1),fill=P['dirt1'])
                    d.line((nx*T+(15 if dx==-1 else 0 if dx==1 else x0),ny*T+(15 if dy==-1 else 0 if dy==1 else y0),nx*T+(15 if dx==-1 else 0 if dx==1 else x1),ny*T+(15 if dy==-1 else 0 if dy==1 else y1)),fill=P['jd4'])
# shadows under cliff
for x in range(7,13):
    if LAY[3][x]=='N':
        for i in range(3):
            d.line((x*T,4*T+i,x*T+15,4*T+i),fill=(8,24,28,90-i*25))
# props sorted by feet y
props=[]
pl=palm(1); pl2=palm(2); bu=bush(); mk=marker(); pi=pillar()
props+= [(pl,40,60),(pl2,24,150),(pl,260,70),(pl2,292,140),(pl,225,170),(pl,110,172),
         (bu,70,100),(bu,215,110),(bu,150,172),(bu,300,52),
         (pi,106,52),(pi,206,52),(mk,152,40)]
# player
player=sprite('up','wA'); props.append((player,153,100))
props.sort(key=lambda p:p[2])
for im,x,fy in props:
    scene.alpha_composite(im,(int(x-im.width//2),int(fy-im.height)))
scene.save(PREVIEWS_DIR / 'scene_flat.png')

# ---- lighting / post ----
a=np.asarray(scene.convert('RGB')).astype(np.float32)/255
h,w,_=a.shape
yy,xx=np.mgrid[0:h,0:w]
# cool teal ambient grade
a=a*np.array([0.80,0.94,1.0])
# god rays from top-left
ray=np.clip(np.sin((xx*0.9+yy*0.5)/9.0)*0.5+0.5,0,1)**3
fade=np.clip(1-(yy/h)*1.1,0,1)*np.clip(1-(xx/w)*0.6,0,1)
a+= (ray*fade*0.10)[...,None]*np.array([1.0,0.9,0.6])
# warm glow at marker
g=np.exp(-(((xx-160)**2+(yy-30)**2)/(2*46**2)))
a+= (g*0.30)[...,None]*np.array([1.0,0.82,0.42])
# soft canopy shade patches
shade=(np.sin(xx/13.0+np.sin(yy/9.0)*2)*0.5+0.5)*(np.sin(yy/11.0+xx/23.0)*0.5+0.5)
a*= (1-0.16*shade)[...,None]
# vignette
v=((xx-w/2)/(w/2))**2+((yy-h/2)/(h/2))**2
a*= (1-0.38*np.clip(v-0.25,0,1))[...,None]
# lift blacks to teal, contrast curve
a=np.clip(a,0,1); a=a**0.95; a=a+np.array([0.01,0.03,0.04])*(1-a)
out=Image.fromarray((np.clip(a,0,1)*255).astype(np.uint8))
# fireflies
od=ImageDraw.Draw(out)
rr=random.Random(3)
for _ in range(16):
    x,y=rr.randrange(w),rr.randrange(h//2+40)
    od.point((x,y),fill=(255,233,150))
out.save(PREVIEWS_DIR / 'scene_lit_320x192.png')
out.resize((w*4,h*4),Image.NEAREST).save(PREVIEWS_DIR / 'preview_scene.png')
scene.resize((w*4,h*4),Image.NEAREST).save(PREVIEWS_DIR / 'preview_flat_4x.png')
for image_path in (tileset_path, player_path):
    im=Image.open(image_path)
    im.resize((im.width*8,im.height*8),Image.NEAREST).save(
        PREVIEWS_DIR / f'{image_path.stem}_8x.png')
print('ok')
