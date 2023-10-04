"""
Automated robot controller
Program Inputs:
1. To start autonomous motion type m
2. To start experiment and avoind occlusions right click********************************
3. To follow robot path modify path variable controller_service.cpp
4. Specify occlusions

Program timers:
1. predator step updates
2. prey step updates
3. controller - destination timer

Mutable Variables:
1. occlusions

TO DO:
1. change random location to "belief state" new location
2. test predator canonical pursuit
5. look at PID fix distance overshoot check normalize error correct
6. added pause and resume to avoid overshoot fix later
7. PREYS LOCATION NOT CANNONICAL FIX THIS!!!
"""

import sys
from cellworld import World, Display, Location, Agent_markers, Capture, Capture_parameters, Step, Timer, Cell_group_builder, Polygon, Polygon_list, Location_visibility, Cell_map, Coordinates
from cellworld_controller_service import ControllerClient
from cellworld_experiment_service import ExperimentClient
from random import choice
from time import sleep


class AgentData:
    """
    Class to initialize prey and predator objects
    """
    def __init__(self, agent_name: str):
        self.is_valid = None # timers for predator and prey updates
        self.step = Step()
        self.step.agent_name = agent_name


def load_world():
    """
    Load world to display
    """
    global display
    global world
    global map

    occlusion = Cell_group_builder.get_from_name("hexagonal", occlusions + ".occlusions")
    world.set_occlusions(occlusion)
    display = Display(world, fig_size=(9.0*.75, 8.0*.75), animated=True)
    map = Cell_map(world.configuration.cell_coordinates)




def on_step(step: Step):
    """
    Updates steps and predator behavior
    """
    global behavior

    if step.agent_name == "predator":
        predator.is_valid = Timer(time_out)
        predator.step = step
        display.circle(step.location, 0.002, "royalblue")    # plot predator path (steps)

    else:
        prey.is_valid = Timer(time_out) # pursue when prey is seen
        prey.step = step



def on_click(event):
    """
    Assign destination by clicking on map
    Right-click to start experiment
    """
    global current_predator_destination
    global destination_list

    if event.button == 1:
        controller.resume()
        location = Location(event.xdata, event.ydata)
        cell_id = world.cells.find(location)
        destination_cell = world.cells[cell_id]
        if destination_cell.occluded:
            print("can't navigate to an occluded cell")
            return
        current_predator_destination = destination_cell.location
        controller.set_destination(destination_cell.location)
        display.circle(current_predator_destination, 0.01, "red")



def on_keypress(event):
    """
    Sets up keyboard intervention
    """
    global running
    global current_predator_destination
    global controller_timer
    global destination_list
    global controller_state

    if event.key == "p":
        print("pause")
        controller.pause()
        controller_state = 0
    if event.key == "r":
        print("resume")
        controller.resume()
        controller_state = 1
    if event.key == "q":
        print("quit")
        controller.pause()
        running = False



# SET UP GLOBAL VARIABLES
occlusions = "00_00"
time_out = 1.0      # step timer for predator and preyQ

display = None

# create world
world = World.get_from_parameters_names("hexagonal", "canonical")
load_world()

#  create predator and prey objects
predator = AgentData("predator")
prey = AgentData("prey")

# set initial destination and behavior
current_predator_destination = predator.step.location  # initial predator destination



# CONNECT TO CONTROLLER
controller_timer = Timer(1)     # initialize controller timer variable
controller = ControllerClient()
if not controller.connect("127.0.0.1", 4590):
    print("failed to connect to the controller")
    exit(1)
controller.set_request_time_out(10000)
controller.subscribe()
controller.on_step = on_step


# INITIALIZE KEYBOARD & CLICK INTERRUPTS
cid1 = display.fig.canvas.mpl_connect('button_press_event', on_click)
cid_keypress = display.fig.canvas.mpl_connect('key_press_event', on_keypress)

# ADD PREDATOR AND PREY TO WORLD
display.set_agent_marker("predator", Agent_markers.arrow())
display.set_agent_marker("prey", Agent_markers.arrow())
destination_circle = display.circle(predator.step.location, 0.01, 'magenta')

running = True
while running:

    controller.set_destination(current_predator_destination)     # set destination
    controller_timer.reset()                                     # reset controller timer
    destination_circle.set(center = (current_predator_destination.x, current_predator_destination.y), color = 'magenta')


    # plotting the current location of the predator and prey
    if prey.is_valid:
        display.agent(step=prey.step, color="green", size=10)

    else:
        display.agent(step=prey.step, color="gray", size=10)

    if predator.is_valid:
        display.agent(step=predator.step, color="blue", size=10)

    else:
        display.agent(step=predator.step, color="gray", size=10)



    display.fig.canvas.draw_idle()
    display.fig.canvas.start_event_loop(0.001)
    sleep(0.1)


controller.unsubscribe()
controller.stop()

