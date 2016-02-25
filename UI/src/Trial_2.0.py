import kivy
kivy.require('1.0.6')

from kivy.app import App

from kivy.clock import Clock
from kivy.lang import Builder
from kivy.uix.widget import Widget
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.uix.progressbar import ProgressBar
from kivy.properties import NumericProperty, BooleanProperty ,StringProperty,ListProperty



import sys
import time

#Globals
ticks=0 # total run time
s_state=0 # screen state-> 0:welcome screen, 1: main screen
heart=0.0# heart rate
breath=0.0 # breathing rate
# Debug values
# optimal values
#h_buffer = [54.4,55.0, 60.5,67.8,65.3,66.2,63.1,63.8,70.0]
#b_buffer = [14.4,15.0, 12.5,17.8,15.3,16.2,13.1,13.8,20.0]

# suboptimal values
#h_buffer = [34.4,35.0, 40.5,37.8,25.3,26.2,23.1,33.8,35.0]
#b_buffer = [8.4,9.0, 7.5,7.8,5.3,7.2,9.1,7.8,8.0]

# over_optimal values
h_buffer = [164.4,185.0, 190.5,187.8,195.3,186.2,183.1,183.8,185.0]
b_buffer = [68.4,69.0, 67.5,67.8,75.3,67.2,59.1,67.8,68.0]


i = 0
first_v= True

# Create both screens. Please note the root.manager.current: this is how
# you can control the ScreenManager from kv. Each screen has by default a
# property manager that gives you the instance of the ScreenManager used.
Builder.load_string("""
<WelcomeScreen>:
    Widget:
        Button:
            size: 80,40
            text: 'Enter'
            pos: 50,50
            color: 153, 153, 102
            on_press:
                root.manager.transition.direction = 'left'
                root.manager.current = 'settings'
                root.setmainstate(self,*args)
        Button:
            text: 'Quit'
            size: 80,40
            color: 153, 153, 102
            pos: root.width-130,50
            on_press:root.close(self,*args)
            
        Label:
            text:'cVitals'
            font_size:'50sp'
            
            color: 0,255,255,1
            bold:True
            pos: (root.width/2)-50,root.height/2

<MainScreen>:
    Widget:
        color: 229, 255, 255
    
        GridLayout:
            id: Heart_layout
            cols:1
            #default design
            rows:3
            #pending  design
            #
            #pos:(root.width/2)-200,(root.height/2)+150)
            pos:150,(root.height/2)+150
            col_default_width: 100
            row_default_width: 50
            row_default_height: 100
            
            Label:
                text:'Heart Rate :'
                color: 0,255,255,1
                bold: True

            Image:
                source: '..\Icons\heart2.gif'
                size: 75,75
            Label:
                
                text: root.heart_rate
                id: heart_label
                color: root.h_monitor
                font_size:'125sp'
               
                

        GridLayout:
            id: Breath_layout
            cols:1
            rows:3
            #pos:(root.width/2)+200,((root.height/2)+150)
            pos: (root.width-250),((root.height/2)+150)
            col_default_width: 100
            row_default_width: 50
            row_default_height: 100
            
            Label:
                text: 'Breathing Rate :'
                color: 0,255,255,1
                bold: True

            Image:
                source: '..\Icons\lungs2.gif'
                size: 75,75
                
            Label:
                text: root.breath_rate
                id: chest_label
                color: root.b_monitor
                font_size:'125sp'

        #Label:
            #text: 'Calibration @ :{}%'.format(int(cal_prog.value))
            #size_hint_x:None
            #font_size:'25sp'
            #pos:(root.width/2)-20,200

        
        #ProgressBar:
            #pos:(root.width/2)-40,150
            #id: cal_prog
            #size_hint_x: 1.0
            #size_hint_y: None
            #value: root.cal_progress
            
        Button:
            text: 'Start'
            id: all_start
            size: 80,40
            pos: 50,50
            color: 153, 153, 102
            on_press:
                
                root.start_cal(self, *args)

        Button:
            text: 'Stop'
            size: 80,40
            pos: (root.width/4)-40,50
            color: 153, 153, 102
            #on_press:
                #root.stop_refresh(self, *args)
            
        

        Button:
            text: 'New Patient'
            size: 150,40
            pos: (root.width * 3/4)-40,50
            color: 153, 153, 102
            on_release:
                root.resetrates(self, *args)
            
        Button:
            text: 'Return'
            size: 80,40
            color: 153, 153, 102
            pos: root.width-130,50
            on_press:
                root.return_clear(self,*args)
                root.manager.transition.direction = 'right'
                root.manager.current = 'menu'
                root.setwelcstate(self,*args)

""")



# Declare both screens
class WelcomeScreen(Screen):
    
    # change screen states so we know which screen we are on
    def setmainstate(self, *args):
        global s_state
        s_state = 1
        
    #exit function
    def close(self,*args):
        sys.exit()
    

class MainScreen(Screen):
    heart_rate = StringProperty()
    breath_rate = StringProperty()
    cal_progress = NumericProperty(0)
    cal_start = BooleanProperty(False)
    cal_done = BooleanProperty(False)

    #rate color settingss
    h_monitor = ListProperty([1,1,1,1])
    b_monitor = ListProperty([1,1,1,1])
    
    
    #allow update of rates in all respects-screen and back-end 
    can_refresh = BooleanProperty(True)
    accept_data = BooleanProperty(False)

    def __init__(self, **kwargs):
        super(MainScreen, self).__init__(**kwargs)
        self.heart_rate = str('N/A')
        self.breath_rate = str('N/A')
        self.cal_progress = 0
        self.cal_start = False
        self.cal_done = False
        Clock.schedule_interval(self._cal_clock, 0.1)
        Clock.schedule_interval(self.ratecalculation, 0.5)
        Clock.schedule_interval(self.refresh_clock, 1)
    
    # On start button press, calibrate 10 sec
    def start_cal(self, *args):
        self.cal_start = True
        self.accept_data = True
        
    def _cal_clock(self, *args):
        #debug
        #print('cal_clock called')
        #print (self.cal_start)
        
        if (self.cal_progress <=100 and self.cal_start and not(self.cal_done)):
            self.cal_progress = (self.cal_progress+1)%101
            if self.cal_progress==100:
                self.cal_done = True

    # screen refresh every 1 sec
    def refresh_clock(self, *args):
        global heart,breath
        if self.cal_done and self.can_refresh:
            tempheart_rate = str (int(heart))
            tempbreath_rate = str (int(breath))
            self.heart_rate = tempheart_rate[:5]
            self.breath_rate = tempbreath_rate[:5]
            if (heart< 39.0 or heart> 170.0):
                self.h_monitor = [255,0,0,1]
            else:
                self.h_monitor = [0,255,255,1]
                
            if (breath < 10.0 or breath >59.0):
                self.b_monitor =[255,0,0,1]
            else:
                self.b_monitor = [0,255,255,1]
        

    # On reset button press,effectively a recalibration
    def resetrates(self, *args):
        print ('Log: Recalibration asserted')
        global heart
        global breath,first_v
        #self.return_clear(self,*args)
        #print('Log: Return to Welc screen.Clear all data and screen')
        #global heart
        #global breath,first_v
        heart = 0.0
        breath = 0.0
        self.h_monitor = [1,1,1,1]#[255,0,0,1]
        self.b_monitor =[1,1,1,1]#[255,0,0,1]
        first_v= True
        self.heart_rate = str('N/A')
        self.breath_rate = str('N/A')
        self.cal_progress = 0
        self.cal_start = False
        self.cal_done = False
        self.accept_data = False
        
        self.start_cal(self, *args)


    # change screen states so we know which screen we are on
    def setwelcstate(self, *args):
        global s_state
        s_state = 0

    # On stop press suspend all logging and refreshing

    def stop_refresh(self, *args):
        self.can_refresh = False
        self.accept_data = False
        print ('Log: Refreshes disabled')
        
    # calculate rates
    def ratecalculation(self, *args):
        global i,breath,heart,h_buffer,b_buffer,first_v

        if(self.can_refresh and self.accept_data):
            if (first_v):
                breath = (breath + b_buffer[i])
                heart = (heart + h_buffer[i])
                first_v =False
                i= (i +1)%10
            else:
                breath = (breath + b_buffer[i])/2
                heart = (heart + h_buffer[i])/2
                (i +1)%10
        print('Heart:' ,heart)
        print('Breath:' , breath)

    # On return: clear screen and reset all data
    
    def return_clear(self,*args):
        print('Log: Return to Welc screen.Clear all data and screen')
        global heart
        global breath,first_v
        heart = 0.0
        breath = 0.0
        self.h_monitor = [1,1,1,1]#[255,0,0,1]
        self.b_monitor =[1,1,1,1]#[255,0,0,1]
        first_v= True
        self.heart_rate = str('N/A')
        self.breath_rate = str('N/A')
        self.cal_progress = 0
        self.cal_start = False
        self.cal_done = False
        self.accept_data = False
        
        
        
    

# Create the screen manager
sm = ScreenManager()
sm.add_widget(WelcomeScreen(name='menu'))
sm.add_widget(MainScreen(name='settings'))

class TestApp(App):
      
    title = 'cVitals'
    #icon = '..\Icons\cVitals-icon3.png'
    def build(self):
        #Clock.schedule_interval(tickup, 1)
         
        Clock.schedule_interval(self.op_clock, 1)
        return sm

    

    
        


#app runtime + rate update interval 
    def op_clock(self, dt):
        #self.time = time()
        #print(self.time)
        global ticks
        global s_state
        ticks=(ticks+1)%10
        print(ticks)
        global s_state
        if s_state == 0:
            print ('Log: Current Screen->welc')
        else:
            print ('Log: Current Screen->main')


#    def tickup(dt):
#        ticks = ticks + 1
#        cal_prog.value = ticks




if __name__ == '__main__':
    TestApp().run()
