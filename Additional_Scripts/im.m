clear all
img=imread('C:\Users\SSDN\Documents\PSOC\edge_detect\skype.png');
x_op = [1, 0, -1; 2, 0, -2;1, 0, -1];
y_op=  [1, 2, 1; 0, 0, 0; -1 -2, -1];
DEPTH=30;
gray_out=zeros(128,128);
for i=1:128
    for j=1:128
      gray_out(i,j)= 0.2989 * img(i,j,1) + 0.5870 * img(i,j,2) + 0.1140 * img(i,j,3);
    end
end
y1= conv2(gray_out,x_op);
y2=conv2(y1,y_op);
sepia=zeros(128,128,3);
for i=1:128
    for j=1:128
    x = (img(i,j,1) + img(i,j,2) +img(i,j,3)) / 3;
    R = x  + 2*DEPTH;
    G  =  x + DEPTH + 4;
    sepia(i,j,3) = x;
    if(R > 255)
      sepia(i,j,1) = 255;
    else
      sepia(i,j,1) = R;
    end
    
    if(G > 255)
      sepia(i,j,2) = 255;
    else
      sepia(i,j,2) =G;
    end
    end
end

 for i=1:128
     for j=1:128      
	  rgb_out(i,j,1) = ( sepia(i,j,1) + y2(i,j))/255;
	  rgb_out(i,j,2)= ( sepia(i,j,2) + y2(i,j))/255;
	  rgb_out(i,j,3) =  (sepia(i,j,3) +y2(i,j))/255;
     end
 end
	