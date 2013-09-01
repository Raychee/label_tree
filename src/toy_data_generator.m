function varargout = toy_data_generator(varargin)
% TOY_DATA_GENERATOR MATLAB code for toy_data_generator.fig
%      TOY_DATA_GENERATOR, by itself, creates a new TOY_DATA_GENERATOR or raises the existing
%      singleton*.
%
%      H = TOY_DATA_GENERATOR returns the handle to a new TOY_DATA_GENERATOR or the handle to
%      the existing singleton*.
%
%      TOY_DATA_GENERATOR('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in TOY_DATA_GENERATOR.M with the given input arguments.
%
%      TOY_DATA_GENERATOR('Property','Value',...) creates a new TOY_DATA_GENERATOR or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before toy_data_generator_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to toy_data_generator_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help toy_data_generator

% Last Modified by GUIDE v2.5 18-Jul-2013 20:22:46

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @toy_data_generator_OpeningFcn, ...
                   'gui_OutputFcn',  @toy_data_generator_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before toy_data_generator is made visible.
function toy_data_generator_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to toy_data_generator (see VARARGIN)

% Choose default command line output for toy_data_generator
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes toy_data_generator wait for user response (see UIRESUME)
% uiwait(handles.visualizer);


% --- Outputs from this function are returned to the command line.
function varargout = toy_data_generator_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on mouse press over axes background.
function cordinate_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to cordinate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
labelrgb = get(hObject, 'UserData');
if isempty(labelrgb) || isempty(labelrgb{1})
    return;
end
label = labelrgb{1};
color = labelrgb{2};
cp = get(hObject, 'CurrentPoint');
x = cp(1, 1); y = cp(1, 2);
line('XData', x, 'YData', y, 'UserData', label, ...
    'Marker', '.', 'MarkerSize', 24, 'MarkerEdgeColor', color);




function editlabel_Callback(hObject, eventdata, handles)
% hObject    handle to editlabel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
label = get(hObject, 'String');
if isnan(label)
    return;
end
labelrgb = get(hObject, 'UserData');
ind = find(strcmp(label, labelrgb(:, 1)), 1);
if isempty(ind)
    ind = find(cellfun(@isempty, labelrgb(:,1)), 1);
    if isempty(ind)
        labelrgb_new = cell(2*size(labelrgb,1), size(labelrgb,2));
        labelrgb_new(1:size(labelrgb,1), :) = labelrgb;
        ind = size(labelrgb, 1) + 1;
        labelrgb = labelrgb_new;
    end
    color = rand(1, 3);
    labelrgb{ind, 1} = label;
    labelrgb{ind, 2} = color;
    set(hObject, 'UserData', labelrgb);
end
set(handles.cordinate, 'UserData', labelrgb(ind, :));


% Hints: get(hObject,'String') returns contents of editlabel as text
%        str2double(get(hObject,'String')) returns contents of editlabel as a double


% --- Executes during object creation, after setting all properties.
function editlabel_CreateFcn(hObject, eventdata, handles)
% hObject    handle to editlabel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
labelrgb = cell(10, 2);
set(hObject, 'UserData', labelrgb);


% --- Executes on button press in savedata.
function savedata_Callback(hObject, eventdata, handles)
% hObject    handle to savedata (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[filename, path] = uiputfile('data.txt', '生成数据文件');
dots = findobj(handles.cordinate, 'Type', 'line');
if isempty(dots) || isempty(filename)
    return;
end
file = fopen([path,filename], 'w');
if file == -1
    return;
end
for i = length(dots) : -1 : 1
    x = get(dots(i), 'XData');
    y = get(dots(i), 'YData');
    label = get(dots(i), 'UserData');
    fprintf(file, '%s 1:%f 2:%f\n', label, x, y);
end
fclose(file);


% --- Executes during object creation, after setting all properties.
function cordinate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to cordinate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate cordinate



function savepath_Callback(hObject, eventdata, handles)
% hObject    handle to savepath (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of savepath as text
%        str2double(get(hObject,'String')) returns contents of savepath as a double


% --- Executes during object creation, after setting all properties.
function savepath_CreateFcn(hObject, eventdata, handles)
% hObject    handle to savepath (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in cleardata.
function cleardata_Callback(hObject, eventdata, handles)
% hObject    handle to cleardata (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
dots = findobj(handles.cordinate, 'Type', 'line');
delete(dots);


% --- Executes on button press in readdata.
function readdata_Callback(hObject, eventdata, handles)
% hObject    handle to readdata (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[filename, path] = uigetfile('*.*', '读取数据文件');
if isempty(path)
    return;
end
file = fopen([path, filename], 'r');
if file == -1
    return;
end
labelrgb = get(handles.editlabel, 'UserData');
while ~feof(file)
    tline = fgetl(file);
    label = sscanf(tline, '%s', 1);
    data = sscanf(tline, '%*s %*d:%f %*d:%f');
    x = data(1); y = data(2);
    ind = find(strcmp(label, labelrgb(:,1)), 1);
    if isempty(ind)
        ind = find(cellfun(@isempty, labelrgb(:,1)), 1);
        if isempty(ind)
            labelrgb_new = cell(2*size(labelrgb,1), size(labelrgb,2));
            labelrgb_new(1:size(labelrgb,1), :) = labelrgb;
            ind = size(labelrgb, 1) + 1;
            labelrgb = labelrgb_new;
        end
        color = rand(1, 3);
        labelrgb{ind, 1} = label;
        labelrgb{ind, 2} = color;
        set(handles.editlabel, 'UserData', labelrgb);
    else
        color = labelrgb{ind, 2};
    end
    line('XData', x, 'YData', y, 'UserData', label, ...
        'Marker', '.', 'MarkerSize', 24, 'MarkerEdgeColor', color);
end
fclose(file);


% --- Executes on button press in readmodel.
function readmodel_Callback(hObject, eventdata, handles)
% hObject    handle to readmodel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[filename, path] = uigetfile('*.*', '读取线性模型');
if isempty(path)
    return;
end
file = fopen([path, filename], 'r');
if file == -1
    return;
end
for i = 1 : 9
    fgetl(file);
end
SV = fscanf(file, '%d', 1) - 1;
fgetl(file);
b = fscanf(file, '%f', 1);
fgetl(file);
xi = zeros(2, SV);
w = zeros(2, 1);
for i = 1 : SV
    alphaiyi = fscanf(file, '%f', 1);
    xi(:, i) = fscanf(file, '%*d:%f %*d:%f #\n', 2);
    w = w + alphaiyi * xi(:, i);
end
fprintf('hyperplane: %g * x + %g * y = %g\n', w(1), w(2), b);
if w(1) > w(2)
    y = 0 : 1;
    x = (b - w(2) .* y) ./ w(1);
else
    x = 0 : 1;
    y = (b - w(1) .* x) ./ w(2);
end
fprintf('line: (%g, %g) -- (%g, %g)\n', x(1), y(1), x(2), y(2));
line(x, y);
fclose(file);
