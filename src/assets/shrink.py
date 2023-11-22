import cv2

file = "AVP_NegativeZ.png"
img=cv2.imread(file)
img = cv2.resize(img, (2048, 2048), interpolation=cv2.INTER_LANCZOS4)
cv2.imwrite(file, img)
