import cv2
# %%
img = cv2.imread("StateNode_Node_Bouton_Left.png",-1)
print(img)
#cv2.imshow("test",img)
#cv2.waitKey(0)
#cv2.destroyAllWindows()
cv2.imwrite("test.png",img)

# %%
