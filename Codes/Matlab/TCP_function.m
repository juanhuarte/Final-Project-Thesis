function TCP_funcion ( numero_muestras , grafico , archivo) 

%variable que cuenta la cantidad de muestras recibidas
contador=1;
%variables de la funcion de sincronismo
sincronizado = 0 ;
marca = 0 ;
%conexion con el servidor
s = tcpclient('192.168.0.12', 80)


while contador <= numero_muestras
    
    %lectura de los bytes que componen una muestra
    out1(contador) = read(s,1,'uint8');
    out2(contador) = read(s,1,'uint8');
    
    %verificacion del sincronizmo
    if (out1(contador) == 255) 
        if (out2(contador) == 255) 
            sincronizado = 1;
            marca=0;  
        elseif (marca == 1) 
            sincronizado =1;
        end
    elseif ( out1(contador) ~= 255 && marca ==1) 
        marca = 0 ;
        sincronizado = 0; 
    elseif (out2(contador) == 255) 
        marca = 1 ;
    end
    if (sincronizado==0 && marca==0)
    muestras (contador)  = uint16 (out2(contador)) * 256 + uint16 (out1(contador));
    contador = contador +1;
    end
    if (sincronizado==1 && marca==1)
        marca = 0;
        sincronizado =0;
        aux = read (s,1,'uint8')
    elseif (sincronizado==1)
        sincronizado =0;
    end
end
    
    % si se pasa el valor de archivo en uno se almacenan los datos en un
    %archivo
    if (archivo == 1)
    dlmwrite('muestras.txt',muestras)
    end
    
    % si grafico esta en uno se realiza el grafico de las muestras
    % recibidas
    if (grafico == 1)
    figure ('Name' , 'Señal de entrada');
    title ('Señal de entrada');
    xlabel ('Muestras');
    ylabel ('Voltaje [V]');
    grid on 
    hold on;
    tiempo = 0:1:length (muestras)-1;
    plot (tiempo, muestras);
    end

end

