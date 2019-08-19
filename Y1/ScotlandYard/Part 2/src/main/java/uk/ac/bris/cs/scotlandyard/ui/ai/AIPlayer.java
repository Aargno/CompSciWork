/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.bris.cs.scotlandyard.ui.ai;

import java.util.HashMap;
import java.util.Map;
import uk.ac.bris.cs.scotlandyard.model.Colour;
import uk.ac.bris.cs.scotlandyard.model.Ticket;

/**
 * Version of ScotlandYardPlayer that does not take a Player object, for simple data storage purposes
 * 
 * @author Limited
 */

public class AIPlayer  {
        
        private final Colour colour; //The players colour
	private int location;
	private final Map<Ticket, Integer> tickets;
        
        public AIPlayer(Colour colour, int location,
			Map<Ticket, Integer> tickets) {
		this.colour = colour;
		this.location = location;
		this.tickets = new HashMap<>(tickets);
	}
        
        /**
         * Returns the colour of the player
         * 
	 * @return the colour of the player
	 */
	public Colour colour() {
		return colour;
	}

	/**
	 * Whether the player is MrX
	 *
	 * @return true if player is MrX, false otherwise
	 */
	public boolean isMrX() {
		return colour.isMrX();
	}

	/**
	 * Whether the player is a detective
	 *
	 * @return true if player is a detective, false otherwise
	 */
	public boolean isDetective() {
		return colour.isDetective();
	}

	/**
	 * Sets the player's current location.
	 *
	 * @param location the location to set
	 */
	public void location(int location) {
		this.location = location;
	}

	/**
         * Updates the players current location
         * 
	 * @return the player's current location.
	 */
	public int location() {
		return location;
	}

	/**
         * Retrieves the tickets held by the player
         * 
	 * @return the player's current tickets.
	 */
	public Map<Ticket, Integer> tickets() {
		return tickets;
	}

	/**
	 * Adds a ticket to the player's current tickets.
	 *
	 * @param ticket the ticket to be added.
	 */
	public void addTicket(Ticket ticket) {
		adjustTicketCount(ticket, 1);
	}

	/**
	 * Removes a ticket to the player's current tickets.
	 *
	 * @param ticket the ticket to be removed.
	 */
	public void removeTicket(Ticket ticket) {
		adjustTicketCount(ticket, -1);
	}
        
        /**
         * Adjusts the players ticket count of the provided ticket type by
         * the integer provided
         * 
         * @param ticket The type of ticket to have it's amount adjusted
         * @param by The number the total tickets of the given type held by the player changes by.
         */
	private void adjustTicketCount(Ticket ticket, int by) {
		Integer ticketCount = tickets.get(ticket);
		ticketCount += by;
		tickets.remove(ticket);
		tickets.put(ticket, ticketCount);
	}

	/**
	 * Checks whether the player has the given ticket
	 *
	 * @param ticket the ticket to check for; not null
	 * @return true if the player has the given ticket, false otherwise
	 */
	public boolean hasTickets(Ticket ticket) {
		return tickets.get(ticket) != 0;
	}

	/**
	 * Checks whether the player has the given ticket and quantity
	 *
	 * @param ticket the ticket to check for; not null
	 * @param quantityInclusive whether the ticket count is greater than or
	 *        equal to given quantity
	 * @return true if the player has the quantity of the given ticket, false
	 *         otherwise
	 */
	public boolean hasTickets(Ticket ticket, int quantityInclusive) {
		return tickets.get(ticket) >= quantityInclusive;
	}
        
        /**
         * Converts the data stored on the player object to a string for easy printing of the objects status
         * 
         * @return String detailing the objects properties
         */
	@Override
	public String toString() {
		final StringBuilder sb = new StringBuilder("AIPlayer{");
		sb.append(", colour=").append(colour);
		sb.append(", location=").append(location);
		sb.append(", tickets=").append(tickets);
		sb.append('}');
		return sb.toString();
	}
	
}
